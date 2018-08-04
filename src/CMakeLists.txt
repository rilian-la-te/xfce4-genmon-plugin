set(PLUGIN_FILES
	cmdspawn.h
	config_gui.h
        #main.c
	cmdspawn.c
	config_gui.c
        mon_widget.c
        mon_widget.h
        launcher.c
        launcher.h
        vala-panel-genmon.c
        vala-panel-genmon.h
)
glib_compile_resources(GLIB_RESOURCES_GENMON
    source
        genmon.gresource.xml
)

add_library(genmon MODULE
                ${PLUGIN_FILES}
                ${GLIB_RESOURCES_GENMON}
        )
target_link_libraries(genmon PRIVATE GLIB2::GIO_UNIX GTK3::GTK PkgConfig::VALAPANEL)
