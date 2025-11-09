include_directories(
    ./lib
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(aurorae_v1 MODULE)
set_target_properties(aurorae_v1 PROPERTIES
    LIBRARY_OUTPUT_NAME "org.kde.kwin.aurorae"
)

target_sources(aurorae_v1 PRIVATE
    aurorae.cpp
    decorationoptions.cpp
    lib/auroraetheme.cpp
    lib/themeconfig.cpp
    renderer.cpp
)

target_compile_definitions(aurorae_v1 PRIVATE
    -DTRANSLATION_DOMAIN=\"aurorae\"
)

target_link_libraries(aurorae_v1
    KDecoration3::KDecoration
    KF6::I18n
    KF6::Package
    KF6::ConfigGui
    Qt::Quick
)

install(TARGETS aurorae_v1 DESTINATION ${KDE_INSTALL_PLUGINDIR}/org.kde.kdecoration3)

add_library(decorationplugin SHARED)
target_sources(decorationplugin PRIVATE
    colorhelper.cpp
    decorationoptions.cpp
    decorationplugin.cpp
)
target_compile_definitions(decorationplugin PRIVATE
    -DTRANSLATION_DOMAIN=\"aurorae\"
)
target_link_libraries(decorationplugin PRIVATE
    KDecoration3::KDecoration
    KF6::ColorScheme
    Qt::Quick
)
install(TARGETS decorationplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kwin/decoration)

install(
    FILES
        qml/AppMenuButton.qml
        qml/AuroraeButton.qml
        qml/AuroraeButtonGroup.qml
        qml/AuroraeMaximizeButton.qml
        qml/Decoration.qml
        qml/DecorationButton.qml
        qml/MenuButton.qml
        qml/aurorae.qml
    DESTINATION
        ${KDE_INSTALL_DATADIR}/kwin/aurorae
)

install(
    FILES
        qml/AppMenuButton.qml
        qml/ButtonGroup.qml
        qml/Decoration.qml
        qml/DecorationButton.qml
        qml/MenuButton.qml
        qml/qmldir
    DESTINATION
        ${KDE_INSTALL_QMLDIR}/org/kde/kwin/decoration
)

add_subdirectory(config)
add_subdirectory(themes)
