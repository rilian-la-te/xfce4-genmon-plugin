glib_compile_resources(GLIB_RESOURCES_GENMON
    source
        genmon.gresource.xml
)
#################
# Vala Panel part
#################
set(PLUGIN_FILES_VALAPANEL
    vala-panel-genmon.c
    vala-panel-genmon.h
    )
if (ENABLE_VALAPANEL)
    #####
    # Plugin Library
    #####
    add_library (genmon-valapanel MODULE
        ${PLUGIN_FILES_VALAPANEL}
        ${GLIB_RESOURCES_GENMON}
    )
    target_link_libraries(genmon-valapanel PRIVATE genmon-core PkgConfig::VALAPANEL)
    set_target_properties(genmon-valapanel PROPERTIES
        OUTPUT_NAME genmon)

    # Install plugin stuffs
    install(TARGETS genmon-valapanel DESTINATION ${CMAKE_INSTALL_LIBDIR}/vala-panel/applets)
endif()
#################
# MATE Panel part
#################
set(PLUGIN_FILES_MATE
    mate-panel-genmon.c
    )
if (ENABLE_MATE)
    #####
    # Plugin Library
    #####
    add_library (genmon-mate MODULE
        ${PLUGIN_FILES_MATE}
        ${GLIB_RESOURCES_GENMON}
    )
    target_link_libraries(genmon-mate PRIVATE genmon-core PkgConfig::MATE)

    # Install plugin stuffs
    install(TARGETS genmon-mate DESTINATION ${CMAKE_INSTALL_LIBDIR}/mate-panel)
endif()
#################
# Xfce Panel part
#################
set(PLUGIN_FILES_XFCE
    xfce4-panel-genmon.c
    xfce4-panel-genmon.h
    )
if (ENABLE_XFCE)
    #####
    # Plugin Library
    #####
    add_library (genmon-xfce MODULE
        ${PLUGIN_FILES_XFCE}
        ${GLIB_RESOURCES_GENMON}
    )
    target_link_libraries(genmon-xfce PRIVATE genmon-core PkgConfig::XFCE)

    # Install plugin stuffs
    install(TARGETS genmon-xfce DESTINATION ${CMAKE_INSTALL_LIBDIR}/xfce4/panel/plugins)
endif()
#################
# Budgie Panel part
#################
set(PLUGIN_FILES_BUDGIE
    budgie-panel-genmon.c
    budgie-panel-genmon.h
    )
if (ENABLE_BUDGIE)
    #####
    # Plugin Library
    #####
    add_library (genmon-budgie MODULE
        ${PLUGIN_FILES_BUDGIE}
        ${GLIB_RESOURCES_GENMON}
    )
    target_link_libraries(genmon-budgie PRIVATE genmon-core PkgConfig::BUDGIE)

    # Install plugin stuffs
    install(TARGETS genmon-budgie DESTINATION ${CMAKE_INSTALL_LIBDIR}/budgie-desktop/plugins)
endif()
