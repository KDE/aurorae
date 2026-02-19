/*
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decoration.h"
#include "decorationbutton.h"
#include "decorationbuttongroup.h"
#include "decorationtheme.h"
#include "decorationthemeprovider.h"

#include <KConfigGroup>
#include <KDecoration3/DecoratedWindow>
#include <KDecoration3/DecorationSettings>
#include <KDecoration3/DecorationShadow>
#include <KPluginFactory>
#include <KSvg/FrameSvg>

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

K_PLUGIN_FACTORY_WITH_JSON(AuroraeDecoFactory,
                           "aurorae.json",
                           registerPlugin<Aurorae::Decoration>();
                           registerPlugin<Aurorae::DecorationThemeProvider>();)

namespace Aurorae
{

Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration3::Decoration(parent, args)
{
    if (!args.isEmpty()) {
        const auto map = args.first().toMap();
        m_themeName = map.value(QStringLiteral("theme")).toString().mid(QLatin1String("__aurorae__svg__").size());
    }
}

Decoration::~Decoration()
{
}

qreal Decoration::buttonSizeFactor() const
{
    return m_buttonSizeFactor;
}

void Decoration::setButtonSizeFactor(qreal factor)
{
    if (m_buttonSizeFactor != factor) {
        m_buttonSizeFactor = factor;
        Q_EMIT buttonSizeFactorChanged();
    }
}

bool Decoration::init()
{
    m_theme = DecorationTheme::open(m_themeName);
    if (!m_theme->isValid()) {
        return false;
    }

    m_auroraerc = KSharedConfig::openConfig(QStringLiteral("auroraerc"));
    updateButtonSizeFactor();

    const qreal devicePixelRatio = window()->scale();
    m_decoration = std::make_unique<KSvg::FrameSvg>();
    m_decoration->setImagePath(m_theme->decorationPath());
    m_decoration->setElementPrefix(QStringLiteral("decoration"));
    m_decoration->setEnabledBorders(KSvg::FrameSvg::AllBorders);
    m_decoration->setDevicePixelRatio(devicePixelRatio);

    m_decorationInactive = std::make_unique<KSvg::FrameSvg>();
    m_decorationInactive->setImagePath(m_theme->decorationPath());
    m_decorationInactive->setElementPrefix(m_decoration->hasElementPrefix(QStringLiteral("decoration-inactive")) ? QStringLiteral("decoration-inactive") : m_decoration->prefix());
    m_decorationInactive->setEnabledBorders(KSvg::FrameSvg::AllBorders);
    m_decorationInactive->setDevicePixelRatio(devicePixelRatio);

    m_decorationMaximized = std::make_unique<KSvg::FrameSvg>();
    m_decorationMaximized->setImagePath(m_theme->decorationPath());
    m_decorationMaximized->setElementPrefix(m_decoration->hasElementPrefix(QStringLiteral("decoration-maximized")) ? QStringLiteral("decoration-maximized") : m_decoration->prefix());
    m_decorationMaximized->setEnabledBorders(KSvg::FrameSvg::NoBorder);
    m_decorationMaximized->setDevicePixelRatio(devicePixelRatio);

    m_decorationMaximizedInactive = std::make_unique<KSvg::FrameSvg>();
    m_decorationMaximizedInactive->setImagePath(m_theme->decorationPath());
    m_decorationMaximizedInactive->setElementPrefix(m_decorationInactive->hasElementPrefix(QStringLiteral("decoration-maximized-inactive")) ? QStringLiteral("decoration-maximized-inactive") : m_decorationMaximized->prefix());
    m_decorationMaximizedInactive->setEnabledBorders(KSvg::FrameSvg::NoBorder);
    m_decorationMaximizedInactive->setDevicePixelRatio(devicePixelRatio);

    if (m_decoration->hasElementPrefix(QStringLiteral("innerborder"))) {
        m_innerBorder = std::make_unique<KSvg::FrameSvg>();
        m_innerBorder->setImagePath(m_theme->decorationPath());
        m_innerBorder->setElementPrefix(QStringLiteral("innerborder"));
        m_innerBorder->setDevicePixelRatio(devicePixelRatio);
    }

    if (m_decoration->hasElementPrefix(QStringLiteral("innerborder-inactive"))) {
        m_innerBorderInactive = std::make_unique<KSvg::FrameSvg>();
        m_innerBorderInactive->setImagePath(m_theme->decorationPath());
        m_innerBorderInactive->setElementPrefix(QStringLiteral("innerborder-inactive"));
        m_innerBorderInactive->setDevicePixelRatio(devicePixelRatio);
    }

    if (m_decoration->hasElementPrefix(QStringLiteral("mask"))) {
        m_blur = std::make_unique<KSvg::FrameSvg>();
        m_blur->setImagePath(m_theme->decorationPath());
        m_blur->setElementPrefix(QStringLiteral("mask"));
        m_blur->setEnabledBorders(KSvg::FrameSvg::AllBorders);
    }

    m_leftButtons = std::make_unique<DecorationButtonGroup>(DecorationButtonGroup::Position::Left, this, std::bind_front(&Decoration::instantiateButton, this));
    m_leftButtons->setSpacing(m_theme->buttonSpacing());
    m_leftButtons->setButtonGroupHover(m_theme->buttonGroupHover());

    m_rightButtons = std::make_unique<DecorationButtonGroup>(DecorationButtonGroup::Position::Right, this, std::bind_front(&Decoration::instantiateButton, this));
    m_rightButtons->setSpacing(m_theme->buttonSpacing());
    m_rightButtons->setButtonGroupHover(m_theme->buttonGroupHover());

    m_buttonsTimer = std::make_unique<QTimer>();
    m_buttonsTimer->setSingleShot(true);
    connect(m_buttonsTimer.get(), &QTimer::timeout, this, &Decoration::positionButtons);
    connect(m_leftButtons.get(), &DecorationButtonGroup::geometryChanged, m_buttonsTimer.get(), qOverload<>(&QTimer::start));
    connect(m_rightButtons.get(), &DecorationButtonGroup::geometryChanged, m_buttonsTimer.get(), qOverload<>(&QTimer::start));

    updateBorders();
    updateResizeOnlyBorders();
    updateSize();
    updateTitleBar();
    updateCaption();
    positionButtons();
    updateDecoration();
    updateInnerBorder();
    updateBlur();

    connect(window(), &KDecoration3::DecoratedWindow::activeChanged, this, &Decoration::onWindowActiveChanged);
    connect(window(), &KDecoration3::DecoratedWindow::sizeChanged, this, &Decoration::onWindowSizeChanged);
    connect(window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &Decoration::onWindowMaximizedChanged);
    connect(window(), &KDecoration3::DecoratedWindow::captionChanged, this, &Decoration::onWindowCaptionChanged);
    connect(window(), &KDecoration3::DecoratedWindow::scaleChanged, this, &Decoration::onWindowScaleChanged);

    connect(settings().get(), &KDecoration3::DecorationSettings::reconfigured, this, &Decoration::onDecorationSettingsChanged);

    connect(this, &KDecoration3::Decoration::bordersChanged, this, &Decoration::onDecorationBordersChanged);

    return true;
}

KDecoration3::DecorationButton *Decoration::instantiateButton(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent) const
{
    switch (type) {
    case KDecoration3::DecorationButtonType::Menu: {
        if (!m_theme->menuButtonPath().isEmpty()) {
            auto button = new MenuDecorationButton(decoration, parent);
            button->setImagePath(m_theme->menuButtonPath(), m_theme->menuButtonSize() * buttonSizeFactor());
            connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
                button->setImplicitSize(m_theme->menuButtonSize() * buttonSizeFactor());
            });
            return button;
        } else {
            auto button = new FallbackMenuDecorationButton(decoration, parent);
            button->setImplicitSize(m_theme->menuButtonSize() * buttonSizeFactor());
            connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
                button->setImplicitSize(m_theme->menuButtonSize() * buttonSizeFactor());
            });
            return button;
        }
    }
    case KDecoration3::DecorationButtonType::ApplicationMenu: {
        if (!m_theme->appMenuButtonPath().isEmpty()) {
            auto button = new AppMenuDecorationButton(decoration, parent);
            button->setImagePath(m_theme->appMenuButtonPath(), m_theme->appMenuButtonSize() * buttonSizeFactor());
            connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
                button->setImplicitSize(m_theme->appMenuButtonSize() * buttonSizeFactor());
            });
            return button;
        } else {
            auto button = new FallbackAppMenuDecorationButton(decoration, parent);
            button->setImplicitSize(m_theme->appMenuButtonSize() * buttonSizeFactor());
            connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
                button->setImplicitSize(m_theme->appMenuButtonSize() * buttonSizeFactor());
            });
            return button;
        }
    }
    case KDecoration3::DecorationButtonType::OnAllDesktops: {
        if (m_theme->allDesktopsButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new OnAllDesktopsDecorationButton(decoration, parent);
        button->setImagePath(m_theme->allDesktopsButtonPath(), m_theme->allDesktopsButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->allDesktopsButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::Minimize: {
        if (m_theme->minimizeButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new MinimizeDecorationButton(decoration, parent);
        button->setImagePath(m_theme->minimizeButtonPath(), m_theme->minimizeButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->minimizeButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::Maximize: {
        if (m_theme->maximizeButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new MaximizeDecorationButton(decoration, parent);
        button->setImagePath(m_theme->maximizeButtonPath(), m_theme->restoreButtonPath(), m_theme->maximizeRestoreButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->maximizeRestoreButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::Close: {
        if (m_theme->closeButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new CloseDecorationButton(decoration, parent);
        button->setImagePath(m_theme->closeButtonPath(), m_theme->closeButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->closeButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::ContextHelp: {
        if (m_theme->helpButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new HelpDecorationButton(decoration, parent);
        button->setImagePath(m_theme->helpButtonPath(), m_theme->helpButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->helpButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::Shade: {
        if (m_theme->shadeButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new ShadeDecorationButton(decoration, parent);
        button->setImagePath(m_theme->shadeButtonPath(), m_theme->shadeButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->shadeButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::KeepBelow: {
        if (m_theme->keepBelowButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new KeepBelowDecorationButton(decoration, parent);
        button->setImagePath(m_theme->keepBelowButtonPath(), m_theme->keepBelowButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->keepBelowButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::KeepAbove: {
        if (m_theme->keepAboveButtonPath().isEmpty()) {
            return nullptr;
        }
        auto button = new KeepAboveDecorationButton(decoration, parent);
        button->setImagePath(m_theme->keepAboveButtonPath(), m_theme->keepAboveButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->keepAboveButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    case KDecoration3::DecorationButtonType::Spacer: {
        auto button = new SpacerButton(decoration, parent);
        button->setImplicitSize(m_theme->spacerButtonSize() * buttonSizeFactor());
        connect(this, &Decoration::buttonSizeFactorChanged, button, [this, button]() {
            button->setImplicitSize(m_theme->spacerButtonSize() * buttonSizeFactor());
        });
        return button;
    }
    default:
        return nullptr;
    }
}

void Decoration::onWindowActiveChanged()
{
    updateDecoration();
    updateInnerBorder();
}

void Decoration::onWindowSizeChanged()
{
    updateSize();
    updateTitleBar();
    positionButtons();
    updateCaption();
    updateBlur();
}

void Decoration::onWindowMaximizedChanged()
{
    updateBorders();
    updateResizeOnlyBorders();
    updateTitleBar();
    positionButtons();
    updateCaption();
    updateDecoration();
    updateBlur();
}

void Decoration::onWindowCaptionChanged()
{
    if (!m_captionRect.isEmpty()) {
        update(m_captionRect);
    }
}

void Decoration::onWindowScaleChanged()
{
    const qreal devicePixelRatio = window()->scale();

    m_decoration->setDevicePixelRatio(devicePixelRatio);
    m_decorationInactive->setDevicePixelRatio(devicePixelRatio);
    m_decorationMaximized->setDevicePixelRatio(devicePixelRatio);
    m_decorationMaximizedInactive->setDevicePixelRatio(devicePixelRatio);

    if (m_innerBorder) {
        m_innerBorder->setDevicePixelRatio(devicePixelRatio);
    }
    if (m_innerBorderInactive) {
        m_innerBorderInactive->setDevicePixelRatio(devicePixelRatio);
    }
}

void Decoration::onDecorationBordersChanged()
{
    updateSize();
    updateTitleBar();
    positionButtons();
    updateCaption();
    updateBlur();
}

void Decoration::onDecorationSettingsChanged()
{
    m_auroraerc->reparseConfiguration();

    updateButtonSizeFactor();
    updateBorders();
    updateResizeOnlyBorders();
    updateTitleBar();
    positionButtons();
}

void Decoration::updateButtonSizeFactor()
{
    const KConfigGroup group(m_auroraerc, m_themeName);
    const int buttonSize = group.readEntry("ButtonSize", 1);
    setButtonSizeFactor(1.0 + (buttonSize - 1) * 0.2);
}

void Decoration::updateBorders()
{
    if (window()->isMaximized()) {
        setBorders(m_theme->maximizedBorders(buttonSizeFactor()));
    } else {
        setBorders(m_theme->borders(settings()->borderSize(), buttonSizeFactor()));
    }
}

void Decoration::updateResizeOnlyBorders()
{
    const qreal extSize = settings()->largeSpacing();
    QMarginsF resizeOnlyBorders(0, extSize, 0, 0);

    if (settings()->borderSize() == KDecoration3::BorderSize::None) {
        if (!window()->isMaximizedHorizontally()) {
            resizeOnlyBorders.setLeft(extSize);
            resizeOnlyBorders.setRight(extSize);
        }
        if (!window()->isMaximizedVertically()) {
            resizeOnlyBorders.setBottom(extSize);
        }
    } else if (settings()->borderSize() == KDecoration3::BorderSize::NoSides && !window()->isMaximizedHorizontally()) {
        resizeOnlyBorders.setLeft(extSize);
        resizeOnlyBorders.setRight(extSize);
    }

    setResizeOnlyBorders(resizeOnlyBorders);
}

void Decoration::updateSize()
{
    if (window()->isMaximized()) {
        m_decorationMaximized->resizeFrame(size());
        m_decorationMaximizedInactive->resizeFrame(size());
    } else {
        m_decoration->resizeFrame(size().grownBy(m_theme->padding()));
        m_decorationInactive->resizeFrame(size().grownBy(m_theme->padding()));
        if (m_blur) {
            m_blur->resizeFrame(size().grownBy(m_theme->padding()).shrunkBy(QMarginsF(1, 1, 1, 1)));
        }
    }

    if (!window()->isMaximized()) {
        if (m_innerBorder) {
            m_innerBorder->resizeFrame(size().grownBy(m_innerBorder->margins()));
        }
        if (m_innerBorderInactive) {
            m_innerBorderInactive->resizeFrame(size().grownBy(m_innerBorderInactive->margins()));
        }
    }
}

void Decoration::updateTitleBar()
{
    if (window()->isMaximized()) {
        const QMarginsF borders = m_theme->maximizedBorders(buttonSizeFactor());
        setTitleBar(QRectF(borders.left(), 0, window()->width(), borders.top()));
    } else {
        const QMarginsF borders = m_theme->borders(settings()->borderSize(), buttonSizeFactor());
        setTitleBar(QRectF(borders.left(), 0, window()->width(), borders.top()));
    }
}

void Decoration::updateCaption()
{
    const qreal top = window()->isMaximized() ? m_theme->titleEdgesMaximized().top() : m_theme->titleEdges().top();
    const qreal left = m_leftButtons->geometry().right() + m_theme->titleBorderLeft();
    const qreal right = m_rightButtons->geometry().left() - m_theme->titleBorderRight();
    const qreal height = std::max(m_theme->titleHeight(), m_theme->buttonHeight());

    const QRectF captionRect(left, top, right - left, height);
    if (m_captionRect == captionRect) {
        return;
    }

    m_captionRect = captionRect;
    update(m_captionRect);
}

void Decoration::positionButtons()
{
    m_buttonsTimer->stop();

    const QMarginsF edges = window()->isMaximized() ? m_theme->titleEdgesMaximized() : m_theme->titleEdges();
    const qreal buttonMargin = window()->isMaximized() ? m_theme->buttonMarginTopMaximized() : m_theme->buttonMarginTop();

    const QRectF innerTitleRect = QRectF(0, 0, size().width(), borders().top()).marginsRemoved(edges);
    m_leftButtons->setPos(QPointF(innerTitleRect.x(), innerTitleRect.y() + buttonMargin));
    m_rightButtons->setPos(QPointF(innerTitleRect.x() + innerTitleRect.width() - m_rightButtons->geometry().width(), innerTitleRect.y() + buttonMargin));
}

void Decoration::updateDecoration()
{
    if (window()->isMaximized()) {
        if (window()->isActive()) {
            setCurrentDecoration(m_decorationMaximized.get());
        } else {
            setCurrentDecoration(m_decorationMaximizedInactive.get());
        }
    } else {
        if (window()->isActive()) {
            setCurrentDecoration(m_decoration.get());
        } else {
            setCurrentDecoration(m_decorationInactive.get());
        }
    }

    updateShadow();
}

void Decoration::updateInnerBorder()
{
    if (window()->isMaximized()) {
        setCurrentInnerBorder(nullptr);
    } else {
        if (window()->isActive()) {
            setCurrentInnerBorder(m_innerBorder.get());
        } else {
            setCurrentInnerBorder(m_innerBorderInactive.get());
        }
    }
}

void Decoration::updateShadow()
{
    if (m_theme->padding().isNull() || window()->isMaximized()) {
        setShadow(nullptr);
        m_shadow = nullptr;
        m_shadowSvg = nullptr;
        return;
    }

    if (m_shadowSvg == m_currentDecoration && m_shadowSvg->frameSize().toSize() == m_shadow->shadow().size()) {
        return;
    }

    QImage image(m_currentDecoration->frameSize().toSize(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    const QRectF innerRect = QRectF(QPointF(), image.deviceIndependentSize()).marginsRemoved(m_theme->padding());
    QPainter painter(&image);
    m_currentDecoration->paintFrame(&painter);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.drawRect(innerRect);
    painter.end();

    auto shadow = std::make_shared<KDecoration3::DecorationShadow>();
    shadow->setShadow(image);
    shadow->setPadding(m_theme->padding());
    shadow->setInnerShadowRect(innerRect);
    setShadow(shadow);

    m_shadow = shadow;
    m_shadowSvg = m_currentDecoration;
}

void Decoration::updateBlur()
{
    if (!m_blur) {
        return;
    }

    if (window()->isMaximized()) {
        setBlurRegion(QRegion(QRect(QPoint(), size().toSize())));
    } else {
        QRegion mask = m_blur->mask();
        mask.translate(QPoint(-m_theme->padding().left() + 1, -m_theme->padding().top() + 1));
        setBlurRegion(mask);
    }
}

void Decoration::setCurrentDecoration(KSvg::FrameSvg *decoration)
{
    if (m_currentDecoration == decoration) {
        return;
    }

    m_currentDecoration = decoration;
    update();

    updateShadow();
}

void Decoration::setCurrentInnerBorder(KSvg::FrameSvg *innerBorder)
{
    if (m_currentInnerBorder == innerBorder) {
        return;
    }

    m_currentInnerBorder = innerBorder;
    update();
}

void Decoration::paint(QPainter *painter, const QRectF &repaintRegion)
{
    if (window()->isMaximized()) {
        m_currentDecoration->paintFrame(painter);
    } else {
        m_currentDecoration->paintFrame(painter, QPointF(-m_theme->padding().left(), -m_theme->padding().top()));
    }

    if (m_currentInnerBorder) {
        m_currentInnerBorder->paintFrame(painter, QPointF(borders().left() - m_currentInnerBorder->marginSize(KSvg::FrameSvg::LeftMargin),
                                                          borders().top() - m_currentDecoration->marginSize(KSvg::FrameSvg::TopMargin)));
    }

    if (m_captionRect.intersects(repaintRegion)) {
        const QString elidedCaption = painter->fontMetrics().elidedText(window()->caption(), Qt::ElideMiddle, m_captionRect.width());
        painter->save();
        painter->setFont(settings()->font());
        painter->setPen(window()->isActive() ? m_theme->activeTextColor() : m_theme->inactiveTextColor());
        painter->drawText(m_captionRect, m_theme->alignment() | m_theme->verticalAlignment() | Qt::TextSingleLine, elidedCaption);
        painter->restore();
    }

    if (m_leftButtons->geometry().intersects(repaintRegion)) {
        m_leftButtons->paint(painter, repaintRegion);
    }
    if (m_rightButtons->geometry().intersects(repaintRegion)) {
        m_rightButtons->paint(painter, repaintRegion);
    }
}

} // namespace Aurorae

#include "decoration.moc"
#include "moc_decoration.cpp"
