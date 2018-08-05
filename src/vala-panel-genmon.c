#include "vala-panel-genmon.h"
#include "mon_widget.h"

struct _GenMonApplet
{
	ValaPanelApplet parent;
};

G_DEFINE_DYNAMIC_TYPE(GenMonApplet, genmon_applet, vala_panel_applet_get_type());

GenMonApplet *genmon_applet_new(ValaPanelToplevel *toplevel, GSettings *settings, const char *uuid)
{
	GenMonApplet *self = GENMON_APPLET(
	    vala_panel_applet_construct(genmon_applet_get_type(), toplevel, settings, uuid));
	g_simple_action_set_enabled(G_SIMPLE_ACTION(g_action_map_lookup_action(
	                                G_ACTION_MAP(self), VALA_PANEL_APPLET_ACTION_CONFIGURE)),
	                            true);
	GenMonWidget *widget = genmon_widget_new();
	g_settings_bind(settings,
	                GENMON_PROP_USE_TITLE,
	                widget,
	                GENMON_PROP_USE_TITLE,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(settings,
	                GENMON_PROP_TITLE_TEXT,
	                widget,
	                GENMON_PROP_TITLE_TEXT,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(settings,
	                GENMON_PROP_UPDATE_PERIOD,
	                widget,
	                GENMON_PROP_UPDATE_PERIOD,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(settings, GENMON_PROP_CMD, widget, GENMON_PROP_CMD, G_SETTINGS_BIND_GET);
	g_settings_bind(settings, GENMON_PROP_FONT, widget, GENMON_PROP_FONT, G_SETTINGS_BIND_GET);

	g_object_bind_property(toplevel,
	                       VP_KEY_ORIENTATION,
	                       widget,
	                       VP_KEY_ORIENTATION,
	                       G_BINDING_SYNC_CREATE);

	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(self));

	return self;
}

static void genmon_applet_init(GenMonApplet *self)
{
}

static void genmon_applet_class_init(GenMonAppletClass *klass)
{
}

static void genmon_applet_class_finalize(GenMonAppletClass *klass)
{
}

/*
 * Plugin functions
 */

struct _GenMonPlugin
{
	ValaPanelAppletPlugin parent;
};

G_DEFINE_DYNAMIC_TYPE(GenMonPlugin, genmon_plugin, vala_panel_applet_plugin_get_type());

static ValaPanelApplet *genmon_plugin_get_applet_widget(ValaPanelAppletPlugin *base,
                                                        ValaPanelToplevel *toplevel,
                                                        GSettings *settings, const char *uuid)
{
	g_return_val_if_fail(toplevel != NULL, NULL);
	g_return_val_if_fail(uuid != NULL, NULL);

	return VALA_PANEL_APPLET(genmon_applet_new(toplevel, settings, uuid));
}

GenMonApplet *genmon_plugin_new(GType object_type)
{
	return GENMON_APPLET(vala_panel_applet_plugin_construct(genmon_applet_get_type()));
}

static void genmon_plugin_class_init(GenMonPluginClass *klass)
{
	((ValaPanelAppletPluginClass *)klass)->get_applet_widget = genmon_plugin_get_applet_widget;
}

static void genmon_plugin_init(GenMonPlugin *self)
{
}

static void genmon_plugin_class_finalize(GenMonPluginClass *klass)
{
}

/*
 * IO Module functions
 */

void g_io_genmon_load(GTypeModule *module)
{
	g_return_if_fail(module != NULL);

	genmon_applet_register_type(module);
	genmon_plugin_register_type(module);

	g_type_module_use(module);
	g_io_extension_point_implement(VALA_PANEL_APPLET_EXTENSION_POINT,
	                               genmon_plugin_get_type(),
	                               "genmon",
	                               10);
}

void g_io_genmon_unload(GIOModule *module)
{
	g_return_if_fail(module != NULL);
}
