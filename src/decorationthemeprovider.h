/*
    SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <KDecoration3/DecorationThemeProvider>

namespace Aurorae
{

class DecorationThemeProvider : public KDecoration3::DecorationThemeProvider
{
    Q_OBJECT

public:
    DecorationThemeProvider(QObject *parent, const KPluginMetaData &data);

    QList<KDecoration3::DecorationThemeMetaData> themes() const override;

private:
    QList<KDecoration3::DecorationThemeMetaData> m_themes;
};

} // namespace Aurorae
