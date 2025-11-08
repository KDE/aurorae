/*
    Library for Aurorae window decoration themes.
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#include "auroraetheme.h"
#include "themeconfig.h"
// Qt
#include <QDebug>
#include <QHash>
#include <QStandardPaths>
// KDE
#include <KConfig>
#include <KConfigGroup>

Q_LOGGING_CATEGORY(AURORAE, "aurorae", QtWarningMsg)

namespace Aurorae
{

/************************************************
 * AuroraeThemePrivate
 ************************************************/
class AuroraeThemePrivate
{
public:
    AuroraeThemePrivate();
    ~AuroraeThemePrivate();
    void initButtonFrame(AuroraeButtonType type);
    QString themeName;
    Aurorae::ThemeConfig themeConfig;
    QHash<AuroraeButtonType, QString> pathes;
    KDecoration3::BorderSize buttonSize;
    QString decorationPath;
};

AuroraeThemePrivate::AuroraeThemePrivate()
    : buttonSize(KDecoration3::BorderSize::Normal)
{
}

AuroraeThemePrivate::~AuroraeThemePrivate()
{
}

void AuroraeThemePrivate::initButtonFrame(AuroraeButtonType type)
{
    QString file(QLatin1String("aurorae/themes/") + themeName + QLatin1Char('/') + AuroraeTheme::mapButtonToName(type) + QLatin1String(".svg"));
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, file);
    if (path.isEmpty()) {
        // let's look for svgz
        file += QLatin1String("z");
        path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, file);
    }
    if (!path.isEmpty()) {
        pathes[type] = path;
    } else {
        qCDebug(AURORAE) << "No button for: " << AuroraeTheme::mapButtonToName(type);
    }
}

/************************************************
 * AuroraeTheme
 ************************************************/
AuroraeTheme::AuroraeTheme(const QString &themeName, QObject *parent)
    : QObject(parent)
    , d(std::make_unique<AuroraeThemePrivate>())
{
    KConfig config(QLatin1String("aurorae/themes/") + themeName + QLatin1Char('/') + themeName + QLatin1String("rc"), KConfig::FullConfig, QStandardPaths::GenericDataLocation);
    loadTheme(themeName, config);

    connect(this, &AuroraeTheme::themeChanged, this, &AuroraeTheme::borderSizesChanged);
    connect(this, &AuroraeTheme::buttonSizesChanged, this, &AuroraeTheme::borderSizesChanged);
}

AuroraeTheme::~AuroraeTheme() = default;

bool AuroraeTheme::isValid() const
{
    return !d->themeName.isNull();
}

void AuroraeTheme::loadTheme(const QString &name, const KConfig &config)
{
    d->themeName = name;
    QString file(QLatin1String("aurorae/themes/") + d->themeName + QLatin1String("/decoration.svg"));
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, file);
    if (path.isEmpty()) {
        file += QLatin1String("z");
        path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, file);
    }
    if (path.isEmpty()) {
        qCDebug(AURORAE) << "Could not find decoration svg: aborting";
        d->themeName.clear();
        return;
    }
    d->decorationPath = path;

    // load the buttons
    d->initButtonFrame(MinimizeButton);
    d->initButtonFrame(MaximizeButton);
    d->initButtonFrame(RestoreButton);
    d->initButtonFrame(CloseButton);
    d->initButtonFrame(AllDesktopsButton);
    d->initButtonFrame(KeepAboveButton);
    d->initButtonFrame(KeepBelowButton);
    d->initButtonFrame(ShadeButton);
    d->initButtonFrame(HelpButton);
    d->initButtonFrame(AppMenuButton);

    d->themeConfig.load(config);
    Q_EMIT themeChanged();
}

bool AuroraeTheme::hasButton(AuroraeButtonType button) const
{
    return d->pathes.contains(button);
}

QLatin1String AuroraeTheme::mapButtonToName(AuroraeButtonType type)
{
    switch (type) {
    case MinimizeButton:
        return QLatin1String("minimize");
    case MaximizeButton:
        return QLatin1String("maximize");
    case RestoreButton:
        return QLatin1String("restore");
    case CloseButton:
        return QLatin1String("close");
    case AllDesktopsButton:
        return QLatin1String("alldesktops");
    case KeepAboveButton:
        return QLatin1String("keepabove");
    case KeepBelowButton:
        return QLatin1String("keepbelow");
    case ShadeButton:
        return QLatin1String("shade");
    case HelpButton:
        return QLatin1String("help");
    case MenuButton:
        return QLatin1String("menu");
    case AppMenuButton:
        return QLatin1String("appmenu");
    default:
        return QLatin1String("");
    }
}

QString AuroraeTheme::themeName() const
{
    return d->themeName;
}

QMarginsF AuroraeTheme::borders(KDecoration3::BorderSize borderSize) const
{
    const qreal titleHeight = std::max((qreal)d->themeConfig.titleHeight(),
                                       d->themeConfig.buttonHeight() * buttonSizeFactor() + d->themeConfig.buttonMarginTop());

    int minMargin;
    int maxMargin;
    switch (borderSize) {
    case KDecoration3::BorderSize::NoSides:
    case KDecoration3::BorderSize::Tiny:
        minMargin = 1;
        maxMargin = 4;
        break;
    case KDecoration3::BorderSize::Normal:
        minMargin = 4;
        maxMargin = 6;
        break;
    case KDecoration3::BorderSize::Large:
        minMargin = 6;
        maxMargin = 8;
        break;
    case KDecoration3::BorderSize::VeryLarge:
        minMargin = 8;
        maxMargin = 12;
        break;
    case KDecoration3::BorderSize::Huge:
        minMargin = 12;
        maxMargin = 20;
        break;
    case KDecoration3::BorderSize::VeryHuge:
        minMargin = 23;
        maxMargin = 30;
        break;
    case KDecoration3::BorderSize::Oversized:
        minMargin = 36;
        maxMargin = 48;
        break;
    default:
        minMargin = 0;
        maxMargin = 0;
    }

    int top = std::clamp(d->themeConfig.borderTop(), minMargin, maxMargin);
    int left = std::clamp(d->themeConfig.borderLeft(), minMargin, maxMargin);
    int right = std::clamp(d->themeConfig.borderRight(), minMargin, maxMargin);
    int bottom = std::clamp(d->themeConfig.borderBottom(), minMargin, maxMargin);

    if (borderSize == KDecoration3::BorderSize::None) {
        left = 0;
        right = 0;
        bottom = 0;
    } else if (borderSize == KDecoration3::BorderSize::NoSides) {
        left = 0;
        right = 0;
    }

    const qreal title = titleHeight + d->themeConfig.titleEdgeTop() + d->themeConfig.titleEdgeBottom();
    switch ((DecorationPosition)d->themeConfig.decorationPosition()) {
    case DecorationTop:
        top = title;
        break;
    case DecorationBottom:
        bottom = title;
        break;
    case DecorationLeft:
        left = title;
        break;
    case DecorationRight:
        right = title;
        break;
    default:
        left = right = bottom = top = 0;
        break;
    }

    return QMarginsF(left, top, right, bottom);
}

QMarginsF AuroraeTheme::maximizedBorders() const
{
    const qreal titleHeight = std::max((qreal)d->themeConfig.titleHeight(),
                                       d->themeConfig.buttonHeight() * buttonSizeFactor() + d->themeConfig.buttonMarginTop());

    const qreal title = titleHeight + d->themeConfig.titleEdgeTopMaximized() + d->themeConfig.titleEdgeBottomMaximized();
    switch ((DecorationPosition)d->themeConfig.decorationPosition()) {
    case DecorationTop:
        return QMarginsF(0, title, 0, 0);
    case DecorationBottom:
        return QMarginsF(0, 0, 0, title);
    case DecorationLeft:
        return QMarginsF(title, 0, 0, 0);
    case DecorationRight:
        return QMarginsF(0, 0, title, 0);
    default:
        return QMarginsF();
    }
}

QMarginsF AuroraeTheme::padding() const
{
    return QMarginsF(d->themeConfig.paddingLeft(), d->themeConfig.paddingTop(), d->themeConfig.paddingRight(), d->themeConfig.paddingBottom());
}

#define THEME_CONFIG(prototype)            \
    int AuroraeTheme::prototype() const    \
    {                                      \
        return d->themeConfig.prototype(); \
    }

THEME_CONFIG(buttonWidth)
THEME_CONFIG(buttonWidthMinimize)
THEME_CONFIG(buttonWidthMaximizeRestore)
THEME_CONFIG(buttonWidthClose)
THEME_CONFIG(buttonWidthAllDesktops)
THEME_CONFIG(buttonWidthKeepAbove)
THEME_CONFIG(buttonWidthKeepBelow)
THEME_CONFIG(buttonWidthShade)
THEME_CONFIG(buttonWidthHelp)
THEME_CONFIG(buttonWidthMenu)
THEME_CONFIG(buttonWidthAppMenu)
THEME_CONFIG(buttonHeight)
THEME_CONFIG(buttonSpacing)
THEME_CONFIG(buttonMarginTop)
THEME_CONFIG(buttonMarginTopMaximized)
THEME_CONFIG(explicitButtonSpacer)
THEME_CONFIG(animationTime)
THEME_CONFIG(titleBorderLeft)
THEME_CONFIG(titleBorderRight)
THEME_CONFIG(titleHeight)

#undef THEME_CONFIG

#define THEME_CONFIG_TYPE(rettype, prototype) \
    rettype AuroraeTheme::prototype() const   \
    {                                         \
        return d->themeConfig.prototype();    \
    }

THEME_CONFIG_TYPE(QColor, activeTextColor)
THEME_CONFIG_TYPE(QColor, inactiveTextColor)
THEME_CONFIG_TYPE(Qt::Alignment, alignment)
THEME_CONFIG_TYPE(Qt::Alignment, verticalAlignment)

#undef THEME_CONFIG_TYPE

QString AuroraeTheme::decorationPath() const
{
    return d->decorationPath;
}

#define BUTTON_PATH(prototype, buttonType)  \
    QString AuroraeTheme::prototype() const \
    {                                       \
        if (hasButton(buttonType)) {        \
            return d->pathes[buttonType];   \
        } else {                            \
            return QString();               \
        }                                   \
    }

BUTTON_PATH(minimizeButtonPath, MinimizeButton)
BUTTON_PATH(maximizeButtonPath, MaximizeButton)
BUTTON_PATH(restoreButtonPath, RestoreButton)
BUTTON_PATH(closeButtonPath, CloseButton)
BUTTON_PATH(allDesktopsButtonPath, AllDesktopsButton)
BUTTON_PATH(keepAboveButtonPath, KeepAboveButton)
BUTTON_PATH(keepBelowButtonPath, KeepBelowButton)
BUTTON_PATH(shadeButtonPath, ShadeButton)
BUTTON_PATH(helpButtonPath, HelpButton)
BUTTON_PATH(appMenuButtonPath, AppMenuButton)

#undef BUTTON_PATH

QMarginsF AuroraeTheme::titleEdges() const
{
    return QMarginsF(d->themeConfig.titleEdgeLeft(), d->themeConfig.titleEdgeTop(), d->themeConfig.titleEdgeRight(), d->themeConfig.titleEdgeBottom());
}

QMarginsF AuroraeTheme::titleEdgesMaximized() const
{
    return QMarginsF(d->themeConfig.titleEdgeLeftMaximized(), d->themeConfig.titleEdgeTopMaximized(), d->themeConfig.titleEdgeRightMaximized(), d->themeConfig.titleEdgeBottomMaximized());
}

void AuroraeTheme::setButtonSize(KDecoration3::BorderSize size)
{
    if (d->buttonSize == size) {
        return;
    }
    d->buttonSize = size;
    Q_EMIT buttonSizesChanged();
}

qreal AuroraeTheme::buttonSizeFactor() const
{
    switch (d->buttonSize) {
    case KDecoration3::BorderSize::Tiny:
        return 0.8;
    case KDecoration3::BorderSize::Large:
        return 1.2;
    case KDecoration3::BorderSize::VeryLarge:
        return 1.4;
    case KDecoration3::BorderSize::Huge:
        return 1.6;
    case KDecoration3::BorderSize::VeryHuge:
        return 1.8;
    case KDecoration3::BorderSize::Oversized:
        return 2.0;
    case KDecoration3::BorderSize::Normal: // fall through
    default:
        return 1.0;
    }
}

DecorationPosition AuroraeTheme::decorationPosition() const
{
    return (DecorationPosition)d->themeConfig.decorationPosition();
}

} // namespace

#include "moc_auroraetheme.cpp"
