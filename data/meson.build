cdata = configuration_data()
cdata.set('CMAKE_INSTALL_FULL_LIBDIR', join_paths(prefix,get_option('libdir')))
cdata.set('CMAKE_INSTALL_FULL_DATAROOTDIR', join_paths(prefix,datadir))

if vala_panel_found
    vp_applets_datadir = vp.get_pkgconfig_variable('applets_data')
    desktop = vcs_tag(
	    command : ['git','describe','--tags'],
	    input: 'genmon.desktop.plugin.in.cmake',
	    output: 'genmon.desktop.plugin.in',
	    replace_string: '@VERSION@',
    )
    i18n.merge_file(
      input: desktop,
      output: 'genmon.plugin',
      type: 'desktop',
      po_dir: join_paths(meson.source_root(),'po'),
      install: true,
      install_dir: vp_applets_datadir
    )
endif

if mate_found
    mate_applet_datadir = join_paths(mp.get_pkgconfig_variable('prefix'),datadir,'mate-panel','applets')
    mate_desktop = configure_file(input : 'genmon-mate.desktop.plugin.in',
               output : 'genmon-mate.desktop.in',
			   configuration : cdata)
    i18n.merge_file(
      input: mate_desktop,
      output: 'org.vala-panel.GenMon.mate-panel-applet',
      type: 'desktop',
      po_dir: join_paths(meson.source_root(),'po'),
      install: true,
      install_dir: mate_applet_datadir
    )
endif

if xfce_found
    xfce_applet_datadir = join_paths(xp.get_pkgconfig_variable('prefix'),datadir,'xfce4','panel','plugins')
    i18n.merge_file(
      input: 'genmon.desktop.xfce.in',
      output: 'genmon.desktop',
      type: 'desktop',
      po_dir: join_paths(meson.source_root(),'po'),
      install: true,
      install_dir: xfce_applet_datadir
    )
endif

if budgie_found
    i18n.merge_file(
      input: 'genmon-budgie.desktop.plugin.in',
      output: 'genmon-budgie.plugin',
      type: 'desktop',
      po_dir: join_paths(meson.source_root(),'po'),
      install: true,
      install_dir: budgie_applet_libdir
    )
endif

if mate_found or vala_panel_found or budgie_found
    install_data([
	    'org.valapanel.toplevel.genmon.gschema.xml'
    ], install_dir: schema_dir)
endif

scripts_dir = join_paths(prefix,get_option('libexecdir'), meson.project_name()) 
script_files = files(
    join_paths('scripts','gmail.ex'),
    join_paths('scripts','monBandwidth'),
    join_paths('scripts','disktemp'),
    join_paths('scripts','datetime'),
    join_paths('scripts','dkspuse'),
    join_paths('scripts','googcal'),
    join_paths('scripts','monTime'),
    join_paths('scripts','twit.ex')
)

monBat = configure_file(input: join_paths('scripts','monBat.in'), output: 'monBat', configuration: cdata, install_dir: scripts_dir)
monBat = configure_file(input: join_paths('scripts','monCPU.in'), output: 'monCPU', configuration: cdata, install_dir: scripts_dir)
monBat = configure_file(input: join_paths('scripts','monUSB.in'), output: 'monUSB', configuration: cdata, install_dir: scripts_dir)
monBat = configure_file(input: join_paths('scripts','monWIFI.in'), output: 'monWIFI', configuration: cdata, install_dir: scripts_dir)
monBat = configure_file(input: join_paths('scripts','sysstat.in'), output: 'sysstat', configuration: cdata, install_dir: scripts_dir)

install_data (
    script_files,
    install_dir: scripts_dir
)
