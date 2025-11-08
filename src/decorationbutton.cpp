/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decorationbutton.h"

#include <KDecoration3/DecoratedWindow>

namespace Aurorae
{

DecorationButton::DecorationButton(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent)
    : KDecoration3::DecorationButton(type, decoration, parent)
{
}

DecorationButton::States DecorationButton::states() const
{
    States states;
    if (decoration()->window()->isActive()) {
        states |= State::Active;
    }
    if (isHovered()) {
        states |= State::Hovered;
    }
    if (isPressed()) {
        states |= State::Pressed;
    }
    if (!isEnabled()) {
        states |= State::Disabled;
    }
    return states;
}

SvgFrameSet SvgFrameSet::from(const QString &imagePath, const QSizeF &implicitSize, qreal devicePixelRatio)
{
    SvgFrameSet svgs;

    svgs.active = std::make_unique<KSvg::FrameSvg>();
    svgs.active->setImagePath(imagePath);
    svgs.active->setElementPrefix(QStringLiteral("active"));
    svgs.active->setDevicePixelRatio(devicePixelRatio);
    svgs.active->resizeFrame(implicitSize);

    if (svgs.active->hasElementPrefix(QStringLiteral("hover"))) {
        svgs.hover = std::make_unique<KSvg::FrameSvg>();
        svgs.hover->setImagePath(imagePath);
        svgs.hover->setElementPrefix(QStringLiteral("hover"));
        svgs.hover->setDevicePixelRatio(devicePixelRatio);
        svgs.hover->resizeFrame(implicitSize);
    }

    if (svgs.active->hasElementPrefix(QStringLiteral("pressed"))) {
        svgs.pressed = std::make_unique<KSvg::FrameSvg>();
        svgs.pressed->setImagePath(imagePath);
        svgs.pressed->setElementPrefix(QStringLiteral("pressed"));
        svgs.pressed->setDevicePixelRatio(devicePixelRatio);
        svgs.pressed->resizeFrame(implicitSize);
    }

    if (svgs.active->hasElementPrefix(QStringLiteral("deactivated"))) {
        svgs.deactivated = std::make_unique<KSvg::FrameSvg>();
        svgs.deactivated->setImagePath(imagePath);
        svgs.deactivated->setElementPrefix(QStringLiteral("deactivated"));
        svgs.deactivated->setDevicePixelRatio(devicePixelRatio);
        svgs.deactivated->resizeFrame(implicitSize);
    }

    if (svgs.active->hasElementPrefix(QStringLiteral("inactive"))) {
        svgs.inactive = std::make_unique<KSvg::FrameSvg>();
        svgs.inactive->setImagePath(imagePath);
        svgs.inactive->setElementPrefix(QStringLiteral("inactive"));
        svgs.inactive->setDevicePixelRatio(devicePixelRatio);
        svgs.inactive->resizeFrame(implicitSize);
    }

    if (svgs.active->hasElementPrefix(QStringLiteral("hover-inactive"))) {
        svgs.hoverInactive = std::make_unique<KSvg::FrameSvg>();
        svgs.hoverInactive->setImagePath(imagePath);
        svgs.hoverInactive->setElementPrefix(QStringLiteral("hover-inactive"));
        svgs.hoverInactive->setDevicePixelRatio(devicePixelRatio);
        svgs.hoverInactive->resizeFrame(implicitSize);
    }

    if (svgs.active->hasElementPrefix(QStringLiteral("pressed-inactive"))) {
        svgs.pressedInactive = std::make_unique<KSvg::FrameSvg>();
        svgs.pressedInactive->setImagePath(imagePath);
        svgs.pressedInactive->setElementPrefix(QStringLiteral("pressed-inactive"));
        svgs.pressedInactive->setDevicePixelRatio(devicePixelRatio);
        svgs.pressedInactive->resizeFrame(implicitSize);
    }

    if (svgs.active->hasElementPrefix(QStringLiteral("deactivated-inactive"))) {
        svgs.deactivatedInactive = std::make_unique<KSvg::FrameSvg>();
        svgs.deactivatedInactive->setImagePath(imagePath);
        svgs.deactivatedInactive->setElementPrefix(QStringLiteral("deactivated-inactive"));
        svgs.deactivatedInactive->setDevicePixelRatio(devicePixelRatio);
        svgs.deactivatedInactive->resizeFrame(implicitSize);
    }

    return svgs;
}

KSvg::FrameSvg *SvgFrameSet::select(DecorationButton::States states) const
{
    if (states & DecorationButton::State::Active) {
        if (states & DecorationButton::State::Pressed) {
            if (pressed) {
                return pressed.get();
            }
        } else {
            if (states & DecorationButton::State::Hovered) {
                if (hover) {
                    return hover.get();
                }
            }

            if (states & DecorationButton::State::Disabled) {
                if (deactivated) {
                    return deactivated.get();
                }
            }
        }

        return active.get();
    } else {
        if (states & DecorationButton::State::Pressed) {
            if (pressedInactive) {
                return pressedInactive.get();
            } else if (pressed) {
                return pressed.get();
            }
        } else {
            if (states & DecorationButton::State::Hovered) {
                if (hoverInactive) {
                    return hoverInactive.get();
                } else if (hover) {
                    return hover.get();
                }
            }

            if (states & DecorationButton::State::Disabled) {
                if (deactivatedInactive) {
                    return deactivatedInactive.get();
                } else if (deactivated) {
                    return deactivated.get();
                }
            }
        }

        if (inactive) {
            return inactive.get();
        }

        return active.get();
    }
}

void SvgFrameSet::setDevicePixelRatio(qreal devicePixelRatio)
{
    active->setDevicePixelRatio(devicePixelRatio);

    if (hover) {
        hover->setDevicePixelRatio(devicePixelRatio);
    }

    if (pressed) {
        pressed->setDevicePixelRatio(devicePixelRatio);
    }

    if (deactivated) {
        deactivated->setDevicePixelRatio(devicePixelRatio);
    }

    if (inactive) {
        inactive->setDevicePixelRatio(devicePixelRatio);
    }

    if (hoverInactive) {
        hoverInactive->setDevicePixelRatio(devicePixelRatio);
    }

    if (pressedInactive) {
        pressedInactive->setDevicePixelRatio(devicePixelRatio);
    }

    if (deactivatedInactive) {
        deactivatedInactive->setDevicePixelRatio(devicePixelRatio);
    }
}

SvgDecorationButton::SvgDecorationButton(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent)
    : DecorationButton(type, decoration, parent)
{
    connect(decoration->window(), &KDecoration3::DecoratedWindow::scaleChanged, this, [this, decoration]() {
        m_svgs.setDevicePixelRatio(decoration->window()->scale());
    });

    connect(decoration->window(), &KDecoration3::DecoratedWindow::activeChanged, this, &SvgDecorationButton::updateFrame);
    connect(this, &KDecoration3::DecorationButton::pressedChanged, this, &SvgDecorationButton::updateFrame);
    connect(this, &KDecoration3::DecorationButton::hoveredChanged, this, &SvgDecorationButton::updateFrame);
    connect(this, &KDecoration3::DecorationButton::enabledChanged, this, &SvgDecorationButton::updateFrame);
}

void SvgDecorationButton::setImagePath(const QString &path, const QSizeF &implicitSize)
{
    m_svgs = SvgFrameSet::from(path, implicitSize, decoration()->window()->scale());

    updateFrame();
    setGeometry(QRectF(QPointF(0, 0), implicitSize));
}

void SvgDecorationButton::setFrame(KSvg::FrameSvg *frame)
{
    if (m_frame != frame) {
        m_frame = frame;
        update();
    }
}

void SvgDecorationButton::updateFrame()
{
    setFrame(m_svgs.select(states()));
}

void SvgDecorationButton::paint(QPainter *painter, const QRectF &repaintArea)
{
    m_frame->paintFrame(painter, geometry().topLeft());
}

CloseDecorationButton::CloseDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : SvgDecorationButton(KDecoration3::DecorationButtonType::Close, decoration, parent)
{
    setVisible(decoration->window()->isCloseable());
    connect(decoration->window(), &KDecoration3::DecoratedWindow::closeableChanged, this, [this, decoration]() {
        setVisible(decoration->window()->isCloseable());
    });
}

MaximizeDecorationButton::MaximizeDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : DecorationButton(KDecoration3::DecorationButtonType::Maximize, decoration, parent)
{
    connect(decoration->window(), &KDecoration3::DecoratedWindow::scaleChanged, this, [this, decoration]() {
        const qreal devicePixelRatio = decoration->window()->scale();

        m_maximize.setDevicePixelRatio(devicePixelRatio);
        if (m_restore) {
            m_restore->setDevicePixelRatio(devicePixelRatio);
        }
    });

    setVisible(decoration->window()->isMaximizeable());
    connect(decoration->window(), &KDecoration3::DecoratedWindow::maximizeableChanged, this, [this, decoration]() {
        setVisible(decoration->window()->isMaximizeable());
    });

    connect(decoration->window(), &KDecoration3::DecoratedWindow::activeChanged, this, &MaximizeDecorationButton::updateFrame);
    connect(decoration->window(), &KDecoration3::DecoratedWindow::maximizedChanged, this, &MaximizeDecorationButton::updateFrame);
    connect(this, &KDecoration3::DecorationButton::pressedChanged, this, &MaximizeDecorationButton::updateFrame);
    connect(this, &KDecoration3::DecorationButton::hoveredChanged, this, &MaximizeDecorationButton::updateFrame);
    connect(this, &KDecoration3::DecorationButton::enabledChanged, this, &MaximizeDecorationButton::updateFrame);
}

void MaximizeDecorationButton::setImagePath(const QString &maximizeImagePath, const QString &restoreImagePath, const QSizeF &implicitSize)
{
    const qreal devicePixelRatio = decoration()->window()->scale();

    m_maximize = SvgFrameSet::from(maximizeImagePath, implicitSize, devicePixelRatio);
    if (!restoreImagePath.isEmpty()) {
        m_restore = SvgFrameSet::from(restoreImagePath, implicitSize, devicePixelRatio);
    }

    updateFrame();
    setGeometry(QRectF(QPointF(0, 0), implicitSize));
}

void MaximizeDecorationButton::setFrame(KSvg::FrameSvg *frame)
{
    if (m_frame != frame) {
        m_frame = frame;
        update();
    }
}

void MaximizeDecorationButton::updateFrame()
{
    if (decoration()->window()->isMaximized()) {
        if (m_restore) {
            setFrame(m_restore->select(states()));
            return;
        }
    }

    setFrame(m_maximize.select(states()));
}

void MaximizeDecorationButton::paint(QPainter *painter, const QRectF &repaintArea)
{
    m_frame->paintFrame(painter, geometry().topLeft());
}

MinimizeDecorationButton::MinimizeDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : SvgDecorationButton(KDecoration3::DecorationButtonType::Minimize, decoration, parent)
{
    setVisible(decoration->window()->isMinimizeable());
    connect(decoration->window(), &KDecoration3::DecoratedWindow::minimizeableChanged, this, [this, decoration]() {
        setVisible(decoration->window()->isMinimizeable());
    });
}

KeepAboveDecorationButton::KeepAboveDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : SvgDecorationButton(KDecoration3::DecorationButtonType::KeepAbove, decoration, parent)
{
}

KeepBelowDecorationButton::KeepBelowDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : SvgDecorationButton(KDecoration3::DecorationButtonType::KeepBelow, decoration, parent)
{
}

OnAllDesktopsDecorationButton::OnAllDesktopsDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : SvgDecorationButton(KDecoration3::DecorationButtonType::OnAllDesktops, decoration, parent)
{
}

HelpDecorationButton::HelpDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : SvgDecorationButton(KDecoration3::DecorationButtonType::ContextHelp, decoration, parent)
{
}

ShadeDecorationButton::ShadeDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : SvgDecorationButton(KDecoration3::DecorationButtonType::Shade, decoration, parent)
{
    setVisible(decoration->window()->isShadeable());
    connect(decoration->window(), &KDecoration3::DecoratedWindow::shadeableChanged, this, [this, decoration]() {
        setVisible(decoration->window()->isShadeable());
    });
}

MenuDecorationButton::MenuDecorationButton(KDecoration3::Decoration *decoration, QObject *parent)
    : DecorationButton(KDecoration3::DecorationButtonType::Menu, decoration, parent)
{
    connect(decoration->window(), &KDecoration3::DecoratedWindow::iconChanged, this, qOverload<>(&KDecoration3::DecorationButton::update));
}

void MenuDecorationButton::setImplicitSize(const QSizeF &size)
{
    setGeometry(QRectF(QPointF(0, 0), size));
}

void MenuDecorationButton::paint(QPainter *painter, const QRectF &repaintArea)
{
    decoration()->window()->icon().paint(painter, geometry().toRect());
}

} // namespace Aurorae

#include "moc_decorationbutton.cpp"
