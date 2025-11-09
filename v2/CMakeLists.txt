# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_library(aurorae_v2 MODULE)
set_target_properties(aurorae_v2 PROPERTIES
    LIBRARY_OUTPUT_NAME "org.kde.kwin.aurorae.v2"
)

target_sources(aurorae_v2 PRIVATE
    decoration.cpp
    decorationbutton.cpp
    decorationtheme.cpp
    decorationthemeprovider.cpp
    logging.cpp
)

target_compile_definitions(aurorae_v2 PRIVATE
    -DTRANSLATION_DOMAIN=\"aurorae\"
)

target_link_libraries(aurorae_v2
    KDecoration3::KDecoration
    KF6::CoreAddons
    KF6::ConfigGui
    KF6::Svg
)

install(TARGETS aurorae_v2 DESTINATION ${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration3)
