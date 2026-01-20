/*
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KDecoration3/DecorationButton>

#include <QColor>

namespace Aurorae
{

class DecorationThemeConfig;

enum DecorationPosition {
    DecorationTop = 0,
    DecorationLeft,
    DecorationRight,
    DecorationBottom,
};

class DecorationTheme : public std::enable_shared_from_this<DecorationTheme>
{
public:
    static std::shared_ptr<DecorationTheme> open(const QString &themeName);
    ~DecorationTheme();

    bool isValid() const;
    QMarginsF borders(KDecoration3::BorderSize borderSize, qreal buttonSizeFactor) const;
    QMarginsF maximizedBorders(qreal buttonSizeFactor) const;
    QMarginsF padding() const;
    QSizeF minimizeButtonSize() const;
    QSizeF maximizeRestoreButtonSize() const;
    QSizeF closeButtonSize() const;
    QSizeF allDesktopsButtonSize() const;
    QSizeF keepAboveButtonSize() const;
    QSizeF keepBelowButtonSize() const;
    QSizeF shadeButtonSize() const;
    QSizeF helpButtonSize() const;
    QSizeF menuButtonSize() const;
    QSizeF appMenuButtonSize() const;
    QSizeF spacerButtonSize() const;
    qreal buttonWidth() const;
    qreal buttonHeight() const;
    qreal buttonSpacing() const;
    bool buttonGroupHover() const;
    qreal buttonMarginTop() const;
    qreal buttonMarginTopMaximized() const;
    int animationTime() const;
    QMarginsF titleEdges() const;
    QMarginsF titleEdgesMaximized() const;
    qreal titleBorderLeft() const;
    qreal titleBorderRight() const;
    qreal titleHeight() const;
    QString decorationPath() const;
    QString minimizeButtonPath() const;
    QString maximizeButtonPath() const;
    QString restoreButtonPath() const;
    QString closeButtonPath() const;
    QString allDesktopsButtonPath() const;
    QString keepAboveButtonPath() const;
    QString keepBelowButtonPath() const;
    QString shadeButtonPath() const;
    QString helpButtonPath() const;
    QString menuButtonPath() const;
    QString appMenuButtonPath() const;
    QColor activeTextColor() const;
    QColor inactiveTextColor() const;
    Qt::Alignment alignment() const;
    Qt::Alignment verticalAlignment() const;
    DecorationPosition decorationPosition() const;

private:
    explicit DecorationTheme(const QString &themeName);

    QString resolveFile(const QString &name) const;

    QString m_themeName;
    QString m_minimizeButtonPath;
    QString m_maximizeButtonPath;
    QString m_restoreButtonPath;
    QString m_closeButtonPath;
    QString m_allDesktopsButtonPath;
    QString m_keepAboveButtonPath;
    QString m_keepBelowButtonPath;
    QString m_shadeButtonPath;
    QString m_helpButtonPath;
    QString m_menuButtonPath;
    QString m_appMenuButtonPath;
    QString m_decorationPath;

    QColor m_activeTextColor;
    QColor m_inactiveTextColor;
    Qt::Alignment m_alignment;
    Qt::Alignment m_verticalAlignment;

    qreal m_borderLeft;
    qreal m_borderRight;
    qreal m_borderBottom;
    qreal m_borderTop;

    qreal m_titleEdgeTop;
    qreal m_titleEdgeBottom;
    qreal m_titleEdgeLeft;
    qreal m_titleEdgeRight;
    qreal m_titleEdgeTopMaximized;
    qreal m_titleEdgeBottomMaximized;
    qreal m_titleEdgeLeftMaximized;
    qreal m_titleEdgeRightMaximized;
    qreal m_titleBorderLeft;
    qreal m_titleBorderRight;
    qreal m_titleHeight;

    qreal m_buttonWidth;
    qreal m_buttonWidthMinimize;
    qreal m_buttonWidthMaximizeRestore;
    qreal m_buttonWidthClose;
    qreal m_buttonWidthAllDesktops;
    qreal m_buttonWidthKeepAbove;
    qreal m_buttonWidthKeepBelow;
    qreal m_buttonWidthShade;
    qreal m_buttonWidthHelp;
    qreal m_buttonWidthMenu;
    qreal m_buttonWidthAppMenu;
    qreal m_buttonHeight;
    qreal m_buttonSpacing;
    bool m_buttonGroupHover;
    qreal m_buttonMarginTop;
    qreal m_buttonMarginTopMaximized;
    qreal m_explicitButtonSpacer;

    qreal m_paddingLeft;
    qreal m_paddingRight;
    qreal m_paddingTop;
    qreal m_paddingBottom;

    int m_animationTime;
    int m_decorationPosition;
};

} // namespace Aurorae
