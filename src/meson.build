core_sources = files(
	'cmdspawn.h',
	'cmdspawn.c',
    'config_gui.h',
	'config_gui.c',
    'util.c',
    'util.h',
    'mon_widget.c',
    'mon_widget.h',
    'definitions.h'
)
res = gnome.compile_resources(
    'genmonres', 'genmon.gresource.xml',
    source_dir: join_paths(meson.source_root(),'data'),
    c_name: 'res'
)
core = static_library( 'core',
                        core_sources, config, version,
                        dependencies: gtk,
                        pic: true,
                     )

core_inc = include_directories('.')

core_dep = declare_dependency(link_whole: core, include_directories: core_inc, dependencies: gtk)

if vala_panel_found
    vp_applets_libdir = vp.get_pkgconfig_variable('applets_dir')
    vp_sources = files('vala-panel-genmon.c','vala-panel-genmon.h')
    vpp = shared_module('genmon', vp_sources, res, config, version,
                    dependencies: [core_dep, vp],
                    install: true,
                    install_dir: vp_applets_libdir
                  )
endif

if mate_found
    mate_applet_libdir = join_paths(mp.get_pkgconfig_variable('libdir'),'mate-panel')
    mp_sources = files('mate-panel-genmon.c')
    mpp = shared_module('genmon-mate', mp_sources, res, config, version,
                    dependencies: [core_dep, mp],
                    install: true,
                    install_dir: mate_applet_libdir
                  )
endif

if xfce_found
    xfce_applet_libdir = join_paths(xp.get_pkgconfig_variable('libdir'),'xfce4','panel','plugins')
    xp_sources = files('xfce4-panel-genmon.c','xfce4-panel-genmon.h')
    xpp = shared_module('genmon-xfce', xp_sources, res, config, version,
                    dependencies: [core_dep, xc,xp],
                    install: true,
                    install_dir: xfce_applet_libdir
                  )
endif

if budgie_found
    budgie_applet_libdir = join_paths(bp.get_pkgconfig_variable('libdir'),'budgie-desktop','plugins','budgie-genmon-plugin')
    bp_sources = files('budgie-panel-genmon.c','budgie-panel-genmon.h')
    bpp = shared_module('genmon-budgie', bp_sources, res, config, version,
                    dependencies: [core_dep, bp],
                    install: true,
                    install_dir: budgie_applet_libdir
                  )
endif
