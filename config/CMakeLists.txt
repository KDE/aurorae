kcoreaddons_add_plugin(kcm_auroraedecoration SOURCES auroraeconfig.cpp INSTALL_NAMESPACE "${KDECORATION_KCM_PLUGIN_DIR}")
target_compile_definitions(kcm_auroraedecoration PRIVATE
    -DTRANSLATION_DOMAIN=\"aurorae\"
)
target_link_libraries(kcm_auroraedecoration
    KF6::ConfigGui
    KF6::KCMUtils
    KF6::I18n
    KDecoration3::KDecoration
    Qt::DBus
    Qt::UiTools
    Qt::Widgets
)
