/*
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KDecoration3/Decoration>
#include <KDecoration3/DecorationButtonGroup>

namespace KSvg
{
class FrameSvg;
}

namespace Aurorae
{

class AuroraeTheme;

class Decoration : public KDecoration3::Decoration
{
    Q_OBJECT

public:
    explicit Decoration(QObject *parent = nullptr, const QVariantList &args = QVariantList());
    ~Decoration() override;

    void paint(QPainter *painter, const QRectF &repaintRegion) override;

public Q_SLOTS:
    bool init() override;

private:
    void onWindowActiveChanged();
    void onWindowSizeChanged();
    void onWindowMaximizedChanged();
    void onWindowCaptionChanged();
    void onWindowScaleChanged();

    void onDecorationBordersChanged();
    void onDecorationSettingsChanged();

    void updateBorders();
    void updateResizeOnlyBorders();
    void updateSize();
    void updateTitleBar();
    void updateCaption();
    void positionButtons();
    void updateDecoration();
    void updateInnerBorder();
    void updateShadow();
    void updateBlur();

    void setCurrentDecoration(KSvg::FrameSvg *decoration);
    void setCurrentInnerBorder(KSvg::FrameSvg *innerBorder);

    std::unique_ptr<AuroraeTheme> m_theme;
    QString m_themeName;

    std::unique_ptr<KSvg::FrameSvg> m_decoration;
    std::unique_ptr<KSvg::FrameSvg> m_decorationInactive;
    std::unique_ptr<KSvg::FrameSvg> m_decorationMaximized;
    std::unique_ptr<KSvg::FrameSvg> m_decorationMaximizedInactive;
    std::unique_ptr<KSvg::FrameSvg> m_innerBorder;
    std::unique_ptr<KSvg::FrameSvg> m_innerBorderInactive;
    std::unique_ptr<KSvg::FrameSvg> m_blur;

    KSvg::FrameSvg *m_currentDecoration = nullptr;
    KSvg::FrameSvg *m_currentInnerBorder = nullptr;

    std::unique_ptr<KDecoration3::DecorationButtonGroup> m_leftButtons;
    std::unique_ptr<KDecoration3::DecorationButtonGroup> m_rightButtons;
    std::unique_ptr<QTimer> m_buttonsTimer;

    QRectF m_captionRect;

    std::shared_ptr<KDecoration3::DecorationShadow> m_shadow;
    KSvg::FrameSvg *m_shadowSvg = nullptr;
};

} // namespace Aurorae
