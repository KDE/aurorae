# SPDX-FileCopyrightText: 2025 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

add_subdirectory(config)

add_library(kwin5_aurorae MODULE)
set_target_properties(kwin5_aurorae PROPERTIES
    LIBRARY_OUTPUT_NAME "org.kde.kwin.aurorae"
)

target_sources(kwin5_aurorae PRIVATE
    decoration.cpp
    decorationbutton.cpp
    decorationthemeprovider.cpp
    lib/auroraetheme.cpp
    lib/themeconfig.cpp
)

target_compile_definitions(kwin5_aurorae PRIVATE
    -DTRANSLATION_DOMAIN=\"aurorae\"
)

target_link_libraries(kwin5_aurorae
    KDecoration3::KDecoration
    KF6::CoreAddons
    KF6::ConfigGui
    KF6::Svg
)

install(TARGETS kwin5_aurorae DESTINATION ${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration3)

configure_file(aurorae.knsrc.cmake ${CMAKE_CURRENT_BINARY_DIR}/aurorae.knsrc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/aurorae.knsrc DESTINATION ${KDE_INSTALL_KNSRCDIR})

configure_file(plasma-apply-aurorae.cmake ${CMAKE_CURRENT_BINARY_DIR}/plasma-apply-aurorae)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/plasma-apply-aurorae
    PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    DESTINATION
        ${KDE_INSTALL_LIBEXECDIR}
)
