/*
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decoration.h"
#include "auroraeshared.h"
#include "decorationbutton.h"
#include "decorationthemeprovider.h"
#include "lib/auroraetheme.h"

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
    m_themeName = findTheme(args).mid(QLatin1String("__aurorae__svg__").size());
}

Decoration::~Decoration()
{
}

bool Decoration::init()
{
    const qreal devicePixelRatio = window()->scale();

    m_theme = std::make_unique<AuroraeTheme>(m_themeName, this);

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
    m_decorationMaximizedInactive->setElementPrefix(m_decorationInactive->hasElementPrefix(QStringLiteral("decoration-maximized-inactive")) ? QStringLiteral("decoration-maximized-inactive") : m_decorationInactive->prefix());
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

    const auto instantiateButton = [this](KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent) -> KDecoration3::DecorationButton * {
        switch (type) {
        case KDecoration3::DecorationButtonType::Close: {
            auto button = new CloseDecorationButton(decoration, parent);
            button->setImagePath(m_theme->closeButtonPath(), QSizeF(m_theme->buttonWidthClose(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::Maximize: {
            auto button = new MaximizeDecorationButton(decoration, parent);
            button->setImagePath(m_theme->maximizeButtonPath(), m_theme->restoreButtonPath(), QSizeF(m_theme->buttonWidthMaximizeRestore(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::Minimize: {
            auto button = new MinimizeDecorationButton(decoration, parent);
            button->setImagePath(m_theme->minimizeButtonPath(), QSizeF(m_theme->buttonWidthMinimize(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::KeepAbove: {
            auto button = new KeepAboveDecorationButton(decoration, parent);
            button->setImagePath(m_theme->keepAboveButtonPath(), QSizeF(m_theme->buttonWidthKeepAbove(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::KeepBelow: {
            auto button = new KeepBelowDecorationButton(decoration, parent);
            button->setImagePath(m_theme->keepBelowButtonPath(), QSizeF(m_theme->buttonWidthKeepBelow(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::OnAllDesktops: {
            auto button = new OnAllDesktopsDecorationButton(decoration, parent);
            button->setImagePath(m_theme->allDesktopsButtonPath(), QSizeF(m_theme->buttonWidthAllDesktops(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::ContextHelp: {
            auto button = new HelpDecorationButton(decoration, parent);
            button->setImagePath(m_theme->helpButtonPath(), QSizeF(m_theme->buttonWidthHelp(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::Shade: {
            auto button = new ShadeDecorationButton(decoration, parent);
            button->setImagePath(m_theme->shadeButtonPath(), QSizeF(m_theme->buttonWidthShade(), m_theme->buttonHeight()));
            return button;
        }
        case KDecoration3::DecorationButtonType::Menu: {
            auto button = new MenuDecorationButton(decoration, parent);
            button->setImplicitSize(QSizeF(m_theme->buttonWidthMenu(), m_theme->buttonHeight()));
            return button;
        }
        default:
            return nullptr;
        }
    };

    m_leftButtons = std::make_unique<KDecoration3::DecorationButtonGroup>(KDecoration3::DecorationButtonGroup::Position::Left, this, instantiateButton);
    m_leftButtons->setSpacing(m_theme->buttonSpacing());
    m_rightButtons = std::make_unique<KDecoration3::DecorationButtonGroup>(KDecoration3::DecorationButtonGroup::Position::Right, this, instantiateButton);
    m_rightButtons->setSpacing(m_theme->buttonSpacing());

    m_buttonsTimer = std::make_unique<QTimer>();
    m_buttonsTimer->setSingleShot(true);
    connect(m_buttonsTimer.get(), &QTimer::timeout, this, &Decoration::positionButtons);
    connect(m_leftButtons.get(), &KDecoration3::DecorationButtonGroup::geometryChanged, m_buttonsTimer.get(), qOverload<>(&QTimer::start));
    connect(m_rightButtons.get(), &KDecoration3::DecorationButtonGroup::geometryChanged, m_buttonsTimer.get(), qOverload<>(&QTimer::start));

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
    updateBorders();
    updateResizeOnlyBorders();
    updateTitleBar();
    positionButtons();
}

void Decoration::updateBorders()
{
    if (window()->isMaximized()) {
        setBorders(m_theme->maximizedBorders());
    } else {
        setBorders(m_theme->borders(settings()->borderSize()));
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
            const QMarginsF margins(m_innerBorder->marginSize(KSvg::FrameSvg::LeftMargin),
                                    m_innerBorder->marginSize(KSvg::FrameSvg::TopMargin),
                                    m_innerBorder->marginSize(KSvg::FrameSvg::RightMargin),
                                    m_innerBorder->marginSize(KSvg::FrameSvg::BottomMargin));
            m_innerBorder->resizeFrame(size().grownBy(margins));
        }
        if (m_innerBorderInactive) {
            const QMarginsF margins(m_innerBorderInactive->marginSize(KSvg::FrameSvg::LeftMargin),
                                    m_innerBorderInactive->marginSize(KSvg::FrameSvg::TopMargin),
                                    m_innerBorderInactive->marginSize(KSvg::FrameSvg::RightMargin),
                                    m_innerBorderInactive->marginSize(KSvg::FrameSvg::BottomMargin));
            m_innerBorderInactive->resizeFrame(size().grownBy(margins));
        }
    }
}

void Decoration::updateTitleBar()
{
    if (window()->isMaximized()) {
        const QMarginsF borders = m_theme->maximizedBorders();
        setTitleBar(QRectF(borders.left(), 0, window()->width(), borders.top()));
    } else {
        const QMarginsF borders = m_theme->borders(settings()->borderSize());
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
    if (window()->isMaximized()) { // FIXME:
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
