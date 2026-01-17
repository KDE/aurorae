cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "6.6.80") # Handled by release scripts
project(aurorae VERSION ${PROJECT_VERSION})

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_DEP_VERSION "6.5.90")
set(QT_MIN_VERSION "6.10.0")
set(KF6_MIN_VERSION "6.22.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(WriteBasicConfigVersionFile)
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
include(KDEGitCommitHooks)

include(ECMDeprecationSettings)
include(ECMSetupVersion)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    DBus
    Quick
    UiTools
    Widgets
)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    ColorScheme
    Config
    CoreAddons
    I18n
    KCMUtils
    NewStuff
    Package
    Svg
)

find_package(KDecoration3 ${PROJECT_DEP_VERSION} CONFIG REQUIRED)

find_package(KF6Svg ${KF6_MIN_VERSION} CONFIG)
set_package_properties(KF6Svg PROPERTIES
    DESCRIPTION "A library for rendering SVG-based themes"
    PURPOSE "QML runtime dependency"
    TYPE RUNTIME
)

check_cxx_compiler_flag(-Wno-unused-parameter COMPILER_UNUSED_PARAMETER_SUPPORTED)
if (COMPILER_UNUSED_PARAMETER_SUPPORTED)
    add_compile_options(-Wno-unused-parameter)
endif()

add_definitions(
    -DQT_NO_KEYWORDS
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_FOREACH

    # Prevent EGL headers from including platform headers, in particular Xlib.h.
    -DMESA_EGL_NO_X11_HEADERS
    -DEGL_NO_X11
    -DEGL_NO_PLATFORM_SPECIFIC_TYPES
)

ecm_set_disabled_deprecation_versions(QT 5.15
    KF 6.9.0
)

add_subdirectory(config)
add_subdirectory(v1)
add_subdirectory(v2)

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX AURORAE
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/AuroraeConfigVersion.cmake"
)
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/Aurorae")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/AuroraeConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/AuroraeConfig.cmake"
                              PATH_VARS KDE_INSTALL_FULL_DATADIR
                              INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/AuroraeConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/AuroraeConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
)

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

ki18n_install(po)

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

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
