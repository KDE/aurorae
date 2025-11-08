/*
    Library for Aurorae window decoration themes.
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#pragma once

// #include "libaurorae_export.h"

#include <QObject>

#include <QLoggingCategory>

#include <KDecoration3/DecorationButton>

Q_DECLARE_LOGGING_CATEGORY(AURORAE)

class KConfig;

namespace Aurorae
{
class AuroraeThemePrivate;
class ThemeConfig;

enum AuroraeButtonType {
    MinimizeButton = 0,
    MaximizeButton,
    RestoreButton,
    CloseButton,
    AllDesktopsButton,
    KeepAboveButton,
    KeepBelowButton,
    ShadeButton,
    HelpButton,
    MenuButton,
    AppMenuButton
};

enum DecorationPosition {
    DecorationTop = 0,
    DecorationLeft,
    DecorationRight,
    DecorationBottom,
};

class /*LIBAURORAE_EXPORT*/ AuroraeTheme : public QObject
{
    Q_OBJECT

public:
    explicit AuroraeTheme(const QString &themeName, QObject *parent = nullptr);
    ~AuroraeTheme() override;

    bool isValid() const;
    QString themeName() const;
    QMarginsF borders(KDecoration3::BorderSize borderSize) const;
    QMarginsF maximizedBorders() const;
    QMarginsF padding() const;
    int buttonWidth() const;
    int buttonWidthMinimize() const;
    int buttonWidthMaximizeRestore() const;
    int buttonWidthClose() const;
    int buttonWidthAllDesktops() const;
    int buttonWidthKeepAbove() const;
    int buttonWidthKeepBelow() const;
    int buttonWidthShade() const;
    int buttonWidthHelp() const;
    int buttonWidthMenu() const;
    int buttonWidthAppMenu() const;
    int buttonHeight() const;
    int buttonSpacing() const;
    int buttonMarginTop() const;
    int buttonMarginTopMaximized() const;
    int explicitButtonSpacer() const;
    int animationTime() const;
    QMarginsF titleEdges() const;
    QMarginsF titleEdgesMaximized() const;
    int titleBorderLeft() const;
    int titleBorderRight() const;
    int titleHeight() const;
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
    QString appMenuButtonPath() const;
    QColor activeTextColor() const;
    QColor inactiveTextColor() const;
    Qt::Alignment alignment() const;
    Qt::Alignment verticalAlignment() const;

    /**
     * @returns true if the theme contains a FrameSvg for specified button.
     */
    bool hasButton(AuroraeButtonType button) const;
    /**
     * Sets the size of the buttons.
     * The available sizes are identical to border sizes, therefore BorderSize is used.
     * @param size The buttons size
     */
    void setButtonSize(KDecoration3::BorderSize size);
    qreal buttonSizeFactor() const;

    DecorationPosition decorationPosition() const;

    // TODO: move to namespace
    static QLatin1String mapButtonToName(AuroraeButtonType type);

Q_SIGNALS:
    void themeChanged();
    void buttonSizesChanged();
    void borderSizesChanged();

private:
    void loadTheme(const QString &name, const KConfig &config);

    const std::unique_ptr<AuroraeThemePrivate> d;
};

} // namespace
