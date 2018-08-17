configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
set(PLUGIN_FILES_CORE
        ${CMAKE_CURRENT_BINARY_DIR}/config.h
	cmdspawn.h
	cmdspawn.c
        config_gui.h
	config_gui.c
        util.c
        util.h
        mon_widget.c
        mon_widget.h
)

add_library(genmon-core STATIC ${PLUGIN_FILES_CORE})
set_property(TARGET genmon-core PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries (genmon-core GLIB2::GIO_UNIX GTK3::GTK)

