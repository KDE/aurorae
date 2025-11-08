/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "decorationthemeprovider.h"

#include <KDesktopFile>
#include <KPluginMetaData>

#include <QDir>
#include <QStandardPaths>

namespace Aurorae
{

DecorationThemeProvider::DecorationThemeProvider(QObject *parent, const KPluginMetaData &data)
    : KDecoration3::DecorationThemeProvider(parent)
{
    QStringList themes;
    const QStringList dirs = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QStringLiteral("aurorae/themes/"), QStandardPaths::LocateDirectory);
    QStringList themeDirectories;
    for (const QString &dir : dirs) {
        QDir directory = QDir(dir);
        for (const QString &themeDir : directory.entryList(QDir::AllDirs | QDir::NoDotAndDotDot)) {
            themeDirectories << dir + themeDir;
        }
    }
    for (const QString &dir : themeDirectories) {
        for (const QString &file : QDir(dir).entryList(QStringList() << QStringLiteral("metadata.desktop"))) {
            themes.append(dir + '/' + file);
        }
    }
    for (const QString &theme : themes) {
        int themeSepIndex = theme.lastIndexOf('/', -1);
        QString themeRoot = theme.left(themeSepIndex);
        int themeNameSepIndex = themeRoot.lastIndexOf('/', -1);
        QString packageName = themeRoot.right(themeRoot.length() - themeNameSepIndex - 1);

        KDesktopFile df(theme);
        QString name = df.readName();
        if (name.isEmpty()) {
            name = packageName;
        }

        KDecoration3::DecorationThemeMetaData data;
        data.setPluginId(data.pluginId());
        data.setThemeName(QLatin1String("__aurorae__svg__") + packageName);
        data.setVisibleName(name);
        m_themes.append(data);
    }
}

QList<KDecoration3::DecorationThemeMetaData> DecorationThemeProvider::themes() const
{
    return m_themes;
}

} // namespace Aurorae

#include "moc_decorationthemeprovider.cpp"
