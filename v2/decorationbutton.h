/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KDecoration3/DecorationButton>
#include <KSvg/FrameSvg>

namespace Aurorae
{

class DecorationButton : public KDecoration3::DecorationButton
{
    Q_OBJECT

public:
    enum class State {
        Inactive = 0x0,
        Active = 0x1,
        Hovered = 0x2,
        Pressed = 0x4,
        Disabled = 0x8,
    };
    Q_DECLARE_FLAGS(States, State)

    DecorationButton(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent = nullptr);

    States states() const;

    void setParentHovered(bool hovered);
    bool isParentHovered() const;

Q_SIGNALS:
    void parentHoveredChanged(bool hovered);

private:
    bool m_parentHovered = false;
};

struct SvgFrameSet
{
    static SvgFrameSet from(const QString &imagePath, const QSizeF &implicitSize, qreal devicePixelRatio);

    KSvg::FrameSvg *select(DecorationButton::States states) const;

    void setImplicitSize(const QSizeF &size);
    void setDevicePixelRatio(qreal devicePixelRatio);

    std::unique_ptr<KSvg::FrameSvg> active;
    std::unique_ptr<KSvg::FrameSvg> hover;
    std::unique_ptr<KSvg::FrameSvg> pressed;
    std::unique_ptr<KSvg::FrameSvg> deactivated;

    std::unique_ptr<KSvg::FrameSvg> inactive;
    std::unique_ptr<KSvg::FrameSvg> hoverInactive;
    std::unique_ptr<KSvg::FrameSvg> pressedInactive;
    std::unique_ptr<KSvg::FrameSvg> deactivatedInactive;
};

class SvgDecorationButton : public DecorationButton
{
    Q_OBJECT

public:
    SvgDecorationButton(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent = nullptr);

    void setImagePath(const QString &imagePath, const QSizeF &implicitSize);
    void setImplicitSize(const QSizeF &implicitSize);

    void paint(QPainter *painter, const QRectF &repaintArea) override;

private:
    void setFrame(KSvg::FrameSvg *frame);
    void updateFrame();

    SvgFrameSet m_svgs;
    KSvg::FrameSvg *m_frame = nullptr;
};

class IconDecorationButton : public DecorationButton
{
    Q_OBJECT

public:
    IconDecorationButton(KDecoration3::DecorationButtonType type, KDecoration3::Decoration *decoration, QObject *parent = nullptr);

    void setImplicitSize(const QSizeF &size);

    void paint(QPainter *painter, const QRectF &repaintArea) override;
};

class CloseDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit CloseDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class MaximizeDecorationButton : public DecorationButton
{
    Q_OBJECT

public:
    explicit MaximizeDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);

    void setImagePath(const QString &maximizeImagePath, const QString &restoreImagePath, const QSizeF &implicitSize);
    void setImplicitSize(const QSizeF &implicitSize);

    void paint(QPainter *painter, const QRectF &repaintArea) override;

private:
    void setFrame(KSvg::FrameSvg *frame);
    void updateFrame();

    SvgFrameSet m_maximize;
    std::optional<SvgFrameSet> m_restore;

    KSvg::FrameSvg *m_frame = nullptr;
};

class MinimizeDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit MinimizeDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class KeepAboveDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit KeepAboveDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class KeepBelowDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit KeepBelowDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class OnAllDesktopsDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit OnAllDesktopsDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class HelpDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit HelpDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class ShadeDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit ShadeDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class MenuDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit MenuDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class FallbackMenuDecorationButton : public IconDecorationButton
{
    Q_OBJECT

public:
    explicit FallbackMenuDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class AppMenuDecorationButton : public SvgDecorationButton
{
    Q_OBJECT

public:
    explicit AppMenuDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class FallbackAppMenuDecorationButton : public IconDecorationButton
{
    Q_OBJECT

public:
    explicit FallbackAppMenuDecorationButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);
};

class SpacerButton : public DecorationButton
{
    Q_OBJECT

public:
    explicit SpacerButton(KDecoration3::Decoration *decoration, QObject *parent = nullptr);

    void setImplicitSize(const QSizeF &size);

    void paint(QPainter *painter, const QRectF &repaintArea) override;
};

} // namespace Aurorae

Q_DECLARE_OPERATORS_FOR_FLAGS(Aurorae::DecorationButton::States)
