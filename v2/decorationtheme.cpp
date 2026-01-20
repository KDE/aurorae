/*
    SPDX-FileCopyrightText: 2009, 2010, 2012 Martin Gräßlin <mgraesslin@kde.org>
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decorationtheme.h"
#include "logging.h"

#include <QGuiApplication>
#include <QScreen>
#include <QStandardPaths>

#include <KConfig>
#include <KConfigGroup>

namespace Aurorae
{

Q_GLOBAL_STATIC(QList<DecorationTheme *>, globalThemes)

std::shared_ptr<DecorationTheme> DecorationTheme::open(const QString &themeName)
{
    for (DecorationTheme *theme : std::as_const(*globalThemes)) {
        if (theme->m_themeName == themeName) {
            return theme->shared_from_this();
        }
    }

    return std::shared_ptr<DecorationTheme>(new DecorationTheme(themeName));
}

DecorationTheme::DecorationTheme(const QString &themeName)
    : m_themeName(themeName)
{
    globalThemes->append(this);

    m_decorationPath = resolveFile(QStringLiteral("decoration"));
    if (m_decorationPath.isEmpty()) {
        qCWarning(AURORAE) << "Could not find decoration svg for" << themeName;
        return;
    }

    m_minimizeButtonPath = resolveFile(QStringLiteral("minimize"));
    m_maximizeButtonPath = resolveFile(QStringLiteral("maximize"));
    m_restoreButtonPath = resolveFile(QStringLiteral("restore"));
    m_closeButtonPath = resolveFile(QStringLiteral("close"));
    m_allDesktopsButtonPath = resolveFile(QStringLiteral("alldesktops"));
    m_keepAboveButtonPath = resolveFile(QStringLiteral("keepabove"));
    m_keepBelowButtonPath = resolveFile(QStringLiteral("keepbelow"));
    m_shadeButtonPath = resolveFile(QStringLiteral("shade"));
    m_helpButtonPath = resolveFile(QStringLiteral("help"));
    m_menuButtonPath = resolveFile(QStringLiteral("menu"));
    m_appMenuButtonPath = resolveFile(QStringLiteral("appmenu"));

    KConfig config(QLatin1String("aurorae/themes/") + themeName + QLatin1Char('/') + themeName + QLatin1String("rc"), KConfig::FullConfig, QStandardPaths::GenericDataLocation);
    KConfigGroup general(&config, QStringLiteral("General"));

    m_activeTextColor = general.readEntry("ActiveTextColor", QColor(Qt::black));
    m_inactiveTextColor = general.readEntry("InactiveTextColor", QColor(Qt::black));

    QString alignment = (general.readEntry("TitleAlignment", "Left")).toLower();
    if (alignment == QLatin1StringView("left")) {
        m_alignment = Qt::AlignLeft;
    } else if (alignment == QLatin1StringView("center")) {
        m_alignment = Qt::AlignHCenter;
    } else {
        m_alignment = Qt::AlignRight;
    }

    alignment = (general.readEntry("TitleVerticalAlignment", "Center")).toLower();
    if (alignment == QLatin1StringView("top")) {
        m_verticalAlignment = Qt::AlignTop;
    } else if (alignment == QLatin1StringView("center")) {
        m_verticalAlignment = Qt::AlignVCenter;
    } else {
        m_verticalAlignment = Qt::AlignBottom;
    }

    m_animationTime = general.readEntry("Animation", 0);
    m_decorationPosition = general.readEntry("DecorationPosition", 0);
    m_buttonGroupHover = general.readEntry("ButtonGroupHover", false);

    // TODO: Drop it when the Xorg session support is dropped.
    qreal scaleFactor = 1;
    QScreen *primary = QGuiApplication::primaryScreen();
    if (primary) {
        const qreal dpi = primary->logicalDotsPerInchX();
        scaleFactor = dpi / 96.0f;
    }

    KConfigGroup border(&config, QStringLiteral("Layout"));
    m_borderLeft = std::round(scaleFactor * border.readEntry("BorderLeft", 5));
    m_borderRight = std::round(scaleFactor * border.readEntry("BorderRight", 5));
    m_borderBottom = std::round(scaleFactor * border.readEntry("BorderBottom", 5));
    m_borderTop = std::round(scaleFactor * border.readEntry("BorderTop", 0));

    m_titleEdgeTop = std::round(scaleFactor * border.readEntry("TitleEdgeTop", 5));
    m_titleEdgeBottom = std::round(scaleFactor * border.readEntry("TitleEdgeBottom", 5));
    m_titleEdgeLeft = std::round(scaleFactor * border.readEntry("TitleEdgeLeft", 5));
    m_titleEdgeRight = std::round(scaleFactor * border.readEntry("TitleEdgeRight", 5));
    m_titleEdgeTopMaximized = std::round(scaleFactor * border.readEntry("TitleEdgeTopMaximized", 0));
    m_titleEdgeBottomMaximized = std::round(scaleFactor * border.readEntry("TitleEdgeBottomMaximized", 0));
    m_titleEdgeLeftMaximized = std::round(scaleFactor * border.readEntry("TitleEdgeLeftMaximized", 0));
    m_titleEdgeRightMaximized = std::round(scaleFactor * border.readEntry("TitleEdgeRightMaximized", 0));
    m_titleBorderLeft = std::round(scaleFactor * border.readEntry("TitleBorderLeft", 5));
    m_titleBorderRight = std::round(scaleFactor * border.readEntry("TitleBorderRight", 5));
    m_titleHeight = std::round(scaleFactor * border.readEntry("TitleHeight", 20));

    m_buttonWidth = border.readEntry("ButtonWidth", 20);
    m_buttonWidthMinimize = std::round(scaleFactor * border.readEntry("ButtonWidthMinimize", m_buttonWidth));
    m_buttonWidthMaximizeRestore = std::round(scaleFactor * border.readEntry("ButtonWidthMaximizeRestore", m_buttonWidth));
    m_buttonWidthClose = std::round(scaleFactor * border.readEntry("ButtonWidthClose", m_buttonWidth));
    m_buttonWidthAllDesktops = std::round(scaleFactor * border.readEntry("ButtonWidthAlldesktops", m_buttonWidth));
    m_buttonWidthKeepAbove = std::round(scaleFactor * border.readEntry("ButtonWidthKeepabove", m_buttonWidth));
    m_buttonWidthKeepBelow = std::round(scaleFactor * border.readEntry("ButtonWidthKeepbelow", m_buttonWidth));
    m_buttonWidthShade = std::round(scaleFactor * border.readEntry("ButtonWidthShade", m_buttonWidth));
    m_buttonWidthHelp = std::round(scaleFactor * border.readEntry("ButtonWidthHelp", m_buttonWidth));
    m_buttonWidthMenu = std::round(scaleFactor * border.readEntry("ButtonWidthMenu", m_buttonWidth));
    m_buttonWidthAppMenu = std::round(scaleFactor * border.readEntry("ButtonWidthAppMenu", m_buttonWidthMenu));
    m_buttonWidth = std::round(m_buttonWidth * scaleFactor);
    m_buttonHeight = std::round(scaleFactor * border.readEntry("ButtonHeight", 20));
    m_buttonSpacing = std::round(scaleFactor * border.readEntry("ButtonSpacing", 5));
    m_buttonMarginTop = std::round(scaleFactor * border.readEntry("ButtonMarginTop", 0));
    m_buttonMarginTopMaximized = std::round(scaleFactor * border.readEntry("ButtonMarginTopMaximized", 0));
    m_explicitButtonSpacer = std::round(scaleFactor * border.readEntry("ExplicitButtonSpacer", 10));

    m_paddingLeft = std::round(scaleFactor * border.readEntry("PaddingLeft", 0));
    m_paddingRight = std::round(scaleFactor * border.readEntry("PaddingRight", 0));
    m_paddingTop = std::round(scaleFactor * border.readEntry("PaddingTop", 0));
    m_paddingBottom = std::round(scaleFactor * border.readEntry("PaddingBottom", 0));
}

DecorationTheme::~DecorationTheme()
{
    globalThemes->removeOne(this);
}

QString DecorationTheme::resolveFile(const QString &name) const
{
    QString file(QLatin1String("aurorae/themes/") + m_themeName + QLatin1Char('/') + name + QLatin1String(".svg"));
    QString path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, file);
    if (path.isEmpty()) {
        // let's look for svgz
        file += QLatin1String("z");
        path = QStandardPaths::locate(QStandardPaths::GenericDataLocation, file);
    }
    return path;
}

bool DecorationTheme::isValid() const
{
    return !m_decorationPath.isNull();
}

QMarginsF DecorationTheme::borders(KDecoration3::BorderSize borderSize, qreal buttonSizeFactor) const
{
    const qreal titleHeight = std::max(m_titleHeight, m_buttonHeight * buttonSizeFactor + m_buttonMarginTop);

    qreal minMargin;
    qreal maxMargin;
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

    qreal top = std::clamp(m_borderTop, minMargin, maxMargin);
    qreal left = std::clamp(m_borderLeft, minMargin, maxMargin);
    qreal right = std::clamp(m_borderRight, minMargin, maxMargin);
    qreal bottom = std::clamp(m_borderBottom, minMargin, maxMargin);

    if (borderSize == KDecoration3::BorderSize::None) {
        left = 0;
        right = 0;
        bottom = 0;
    } else if (borderSize == KDecoration3::BorderSize::NoSides) {
        left = 0;
        right = 0;
    }

    const qreal title = titleHeight + m_titleEdgeTop + m_titleEdgeBottom;
    switch ((DecorationPosition)m_decorationPosition) {
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

QMarginsF DecorationTheme::maximizedBorders(qreal buttonSizeFactor) const
{
    const qreal titleHeight = std::max(m_titleHeight, m_buttonHeight * buttonSizeFactor + m_buttonMarginTop);

    const qreal title = titleHeight + m_titleEdgeTopMaximized + m_titleEdgeBottomMaximized;
    switch ((DecorationPosition)m_decorationPosition) {
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

QMarginsF DecorationTheme::padding() const
{
    return QMarginsF(m_paddingLeft, m_paddingTop, m_paddingRight, m_paddingBottom);
}

QSizeF DecorationTheme::minimizeButtonSize() const
{
    return QSizeF(m_buttonWidthMinimize, m_buttonHeight);
}

QSizeF DecorationTheme::maximizeRestoreButtonSize() const
{
    return QSizeF(m_buttonWidthMaximizeRestore, m_buttonHeight);
}

QSizeF DecorationTheme::closeButtonSize() const
{
    return QSizeF(m_buttonWidthClose, m_buttonHeight);
}

QSizeF DecorationTheme::allDesktopsButtonSize() const
{
    return QSizeF(m_buttonWidthAllDesktops, m_buttonHeight);
}

QSizeF DecorationTheme::keepAboveButtonSize() const
{
    return QSizeF(m_buttonWidthKeepAbove, m_buttonHeight);
}

QSizeF DecorationTheme::keepBelowButtonSize() const
{
    return QSizeF(m_buttonWidthKeepBelow, m_buttonHeight);
}

QSizeF DecorationTheme::shadeButtonSize() const
{
    return QSizeF(m_buttonWidthShade, m_buttonHeight);
}

QSizeF DecorationTheme::helpButtonSize() const
{
    return QSizeF(m_buttonWidthHelp, m_buttonHeight);
}

QSizeF DecorationTheme::menuButtonSize() const
{
    return QSizeF(m_buttonWidthMenu, m_buttonHeight);
}

QSizeF DecorationTheme::appMenuButtonSize() const
{
    return QSizeF(m_buttonWidthAppMenu, m_buttonHeight);
}

QSizeF DecorationTheme::spacerButtonSize() const
{
    return QSizeF(m_explicitButtonSpacer, m_buttonHeight);
}

qreal DecorationTheme::buttonWidth() const
{
    return m_buttonWidth;
}

qreal DecorationTheme::buttonHeight() const
{
    return m_buttonHeight;
}

qreal DecorationTheme::buttonSpacing() const
{
    return m_buttonSpacing;
}

bool DecorationTheme::buttonGroupHover() const
{
    return m_buttonGroupHover;
}

qreal DecorationTheme::buttonMarginTop() const
{
    return m_buttonMarginTop;
}

qreal DecorationTheme::buttonMarginTopMaximized() const
{
    return m_buttonMarginTopMaximized;
}

int DecorationTheme::animationTime() const
{
    return m_animationTime;
}

qreal DecorationTheme::titleBorderLeft() const
{
    return m_titleBorderLeft;
}

qreal DecorationTheme::titleBorderRight() const
{
    return m_titleBorderRight;
}

qreal DecorationTheme::titleHeight() const
{
    return m_titleHeight;
}

QColor DecorationTheme::activeTextColor() const
{
    return m_activeTextColor;
}

QColor DecorationTheme::inactiveTextColor() const
{
    return m_inactiveTextColor;
}

Qt::Alignment DecorationTheme::alignment() const
{
    return m_alignment;
}

Qt::Alignment DecorationTheme::verticalAlignment() const
{
    return m_verticalAlignment;
}

QString DecorationTheme::decorationPath() const
{
    return m_decorationPath;
}

QString DecorationTheme::minimizeButtonPath() const
{
    return m_minimizeButtonPath;
}

QString DecorationTheme::maximizeButtonPath() const
{
    return m_maximizeButtonPath;
}

QString DecorationTheme::restoreButtonPath() const
{
    return m_restoreButtonPath;
}

QString DecorationTheme::closeButtonPath() const
{
    return m_closeButtonPath;
}

QString DecorationTheme::allDesktopsButtonPath() const
{
    return m_allDesktopsButtonPath;
}

QString DecorationTheme::keepAboveButtonPath() const
{
    return m_keepAboveButtonPath;
}

QString DecorationTheme::keepBelowButtonPath() const
{
    return m_keepBelowButtonPath;
}

QString DecorationTheme::shadeButtonPath() const
{
    return m_shadeButtonPath;
}

QString DecorationTheme::helpButtonPath() const
{
    return m_helpButtonPath;
}

QString DecorationTheme::menuButtonPath() const
{
    return m_menuButtonPath;
}

QString DecorationTheme::appMenuButtonPath() const
{
    return m_appMenuButtonPath;
}

QMarginsF DecorationTheme::titleEdges() const
{
    return QMarginsF(m_titleEdgeLeft, m_titleEdgeTop, m_titleEdgeRight, m_titleEdgeBottom);
}

QMarginsF DecorationTheme::titleEdgesMaximized() const
{
    return QMarginsF(m_titleEdgeLeftMaximized, m_titleEdgeTopMaximized, m_titleEdgeRightMaximized, m_titleEdgeBottomMaximized);
}

DecorationPosition DecorationTheme::decorationPosition() const
{
    return (DecorationPosition)m_decorationPosition;
}

} // namespace Aurorae
