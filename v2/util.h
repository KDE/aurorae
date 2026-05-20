/*
    SPDX-FileCopyrightText: 2026 Vlad Zahorodnii <vlad.zahorodnii@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QtGlobal>

namespace Aurorae
{

static inline qreal frameSvgScale(qreal scale)
{
    // FrameSvg rendering breaks with scale factors less than 100%.
    if (scale < 1) {
        return 1;
    } else {
        return scale;
    }
}

}
