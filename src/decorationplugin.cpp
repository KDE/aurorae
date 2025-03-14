/*
    SPDX-FileCopyrightText: 2012 Martin Gräßlin <mgraesslin@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "decorationplugin.h"
#include "colorhelper.h"
#include "decorationoptions.h"
#include <QQmlEngine>

void DecorationPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.kwin.decoration"));
    qmlRegisterType<ColorHelper>(uri, 0, 1, "ColorHelper");
    qmlRegisterType<Aurorae::DecorationOptions>(uri, 0, 1, "DecorationOptions");
    qmlRegisterType<Aurorae::Borders>(uri, 0, 1, "Borders");
}

#include "moc_decorationplugin.cpp"
