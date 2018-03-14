set(PLUGIN_FILES
	cmdspawn.h
	config_gui.h
	main.c
	cmdspawn.c
	config_gui.c
)

add_library(genmon-xfce MODULE
		${PLUGIN_FILES}
        )
target_link_libraries(genmon-xfce PRIVATE GLIB2::GIO_UNIX GTK3::GTK)
