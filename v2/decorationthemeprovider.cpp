/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decorationthemeprovider.h"
#include "logging.h"

#include <KDesktopFile>
#include <KPluginMetaData>

#include <QDir>
#include <QStandardPaths>

namespace Aurorae
{

DecorationThemeProvider::DecorationThemeProvider(QObject *parent, const KPluginMetaData &data)
    : KDecoration3::DecorationThemeProvider(parent)
{
    const QStringList themesRoots = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("aurorae/themes/"), QStandardPaths::LocateDirectory);
    for (const QString &themesRoot : themesRoots) {
        const QStringList themeNames = QDir(themesRoot).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
        for (const QString &themeName : themeNames) {
            const QDir themeDirectory(themesRoot + themeName);
            if (!themeDirectory.exists(QStringLiteral("metadata.desktop"))) {
                qCWarning(AURORAE) << themeDirectory.absolutePath() << "has no metadata.desktop file";
                continue;
            }

            const KDesktopFile desktopFile(themeDirectory.filePath(QStringLiteral("metadata.desktop")));
            QString visibleName = desktopFile.readName();
            if (visibleName.isEmpty()) {
                visibleName = themeName;
            }

            KDecoration3::DecorationThemeMetaData theme;
            theme.setPluginId(data.pluginId());
            theme.setThemeName(QLatin1String("__aurorae__svg__") + themeName);
            theme.setVisibleName(visibleName);
            theme.setConfigurationName(QStringLiteral("kcm_auroraedecoration"));
            m_themes.append(theme);
        }
    }
}

QList<KDecoration3::DecorationThemeMetaData> DecorationThemeProvider::themes() const
{
    return m_themes;
}

} // namespace Aurorae

#include "moc_decorationthemeprovider.cpp"
