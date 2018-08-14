/*
 * generic-monitor-plugin
 * Copyright (C) 2015-2018 Konstantin Pugin <ria.freelander@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "vala-panel-genmon.h"
#include "config_gui.h"
#include "mon_widget.h"

#define BORDER 2

struct _GenMonApplet
{
	ValaPanelApplet parent;
	GenMonWidget *widget;
};

G_DEFINE_DYNAMIC_TYPE(GenMonApplet, genmon_applet, vala_panel_applet_get_type());

GenMonApplet *genmon_applet_new(ValaPanelToplevel *toplevel, GSettings *settings, const char *uuid)
{
	GenMonApplet *self = GENMON_APPLET(
	    vala_panel_applet_construct(genmon_applet_get_type(), toplevel, settings, uuid));
	GActionMap *map = G_ACTION_MAP(vala_panel_applet_get_action_group(VALA_PANEL_APPLET(self)));
	g_simple_action_set_enabled(
	    G_SIMPLE_ACTION(g_action_map_lookup_action(map, VALA_PANEL_APPLET_ACTION_CONFIGURE)),
	    true);
	g_simple_action_set_enabled(
	    G_SIMPLE_ACTION(g_action_map_lookup_action(map, VALA_PANEL_APPLET_ACTION_REMOTE)),
	    true);
	GenMonWidget *widget = genmon_widget_new();
	self->widget         = widget;
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

static GtkWidget *genmon_applet_get_settings_ui(ValaPanelApplet *base)
{
	GenMonApplet *self = GENMON_APPLET(base);
	struct gui_t gui_widgets; /* Configuration/option dialog */

	GtkBox *vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, BORDER + 6));
	gtk_container_set_border_width(GTK_CONTAINER(vbox), BORDER + 4);

	(void)genmon_CreateConfigGUI(GTK_WIDGET(vbox), &gui_widgets);

	g_settings_bind(vala_panel_applet_get_settings(base),
	                GENMON_PROP_TITLE_TEXT,
	                gui_widgets.wTF_Title,
	                "text",
	                G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(vala_panel_applet_get_settings(base),
	                GENMON_PROP_CMD,
	                gui_widgets.wTF_Cmd,
	                "text",
	                G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(vala_panel_applet_get_settings(base),
	                GENMON_PROP_FONT,
	                gui_widgets.wPB_Font,
	                "font",
	                G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(vala_panel_applet_get_settings(base),
	                GENMON_PROP_UPDATE_PERIOD,
	                gui_widgets.wSc_Period,
	                "value",
	                G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(vala_panel_applet_get_settings(base),
	                GENMON_PROP_USE_TITLE,
	                gui_widgets.wTB_Title,
	                "active",
	                G_SETTINGS_BIND_DEFAULT);

	g_object_bind_property(gui_widgets.wTB_Title,
	                       "active",
	                       gui_widgets.wTF_Title,
	                       "sensitive",
	                       (GBindingFlags)(G_BINDING_SYNC_CREATE));

	gtk_widget_show_all(GTK_WIDGET(vbox));

	return GTK_WIDGET(vbox);
}

static bool genmon_applet_remote_command(ValaPanelApplet *base, const char *command)
{
	GenMonApplet *self = GENMON_APPLET(base);
	if (g_strcmp0(command, "refresh") == 0)
	{
		genmon_widget_display_command_output(self->widget);
		return true;
	}
	return false;
}

static void genmon_applet_init(GenMonApplet *self)
{
}

static void genmon_applet_class_init(GenMonAppletClass *klass)
{
	((ValaPanelAppletClass *)klass)->get_settings_ui = genmon_applet_get_settings_ui;
	((ValaPanelAppletClass *)klass)->remote_command  = genmon_applet_remote_command;
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
