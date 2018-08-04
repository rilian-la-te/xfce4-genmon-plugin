project(xfce4-genmon-plugin C)
cmake_minimum_required(VERSION 3.6)
cmake_policy (VERSION 3.6)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

find_package(PkgConfig)
set (GETTEXT_PACKAGE "xfce4-genmon-plugin")
include(GitVersion)
determine_fallback_version(${CMAKE_SOURCE_DIR}/cmake)
include(FallbackVersion)
include(Format)

add_definitions(-DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\")
add_definitions(-Wall -Wextra
                -Wno-unused
                -Wno-unused-parameter# Vala loves to create variables
#				-pedantic
                -Wno-ignored-qualifiers
                -Wno-incompatible-pointer-types
                -Wno-missing-field-initializers
                -Wno-sign-compare
                ) #Vala often forgot cast to GtkWidget or gpointer
find_package (PkgConfig)
find_package(GLIB2 2.50 REQUIRED COMPONENTS GOBJECT GIO GIO_UNIX GMODULE GTHREAD CODEGEN MKENUMS GENMARSHAL COMPILE_SCHEMAS COMPILE_RESOURCES)
include(GLibProgramHandlers)
find_package(GTK 3.22 REQUIRED COMPONENTS GDK X11 WAYLAND)
include(CMakeDependentOption)

#set build options
pkg_check_modules(VALAPANEL vala-panel>=0.4.50 IMPORTED_TARGET)
cmake_dependent_option(ENABLE_VALAPANEL "Enable Vala Panel Integration" ON
    "VALAPANEL_FOUND" OFF)

add_subdirectory(data)
add_subdirectory(po)
add_subdirectory(src)
