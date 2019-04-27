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

G_DEFINE_DYNAMIC_TYPE(GenMonApplet, genmon_applet, vala_panel_applet_get_type())

GenMonApplet *genmon_applet_new(ValaPanelToplevel *toplevel, GSettings *settings, const char *uuid)
{
	GenMonApplet *self = GENMON_APPLET(
	    vala_panel_applet_construct(genmon_applet_get_type(), toplevel, settings, uuid));
	return self;
}
static void genmon_applet_constructed(GObject *obj)
{
	G_OBJECT_CLASS(genmon_applet_parent_class)->constructed(obj);
	GenMonApplet *self          = GENMON_APPLET(obj);
	ValaPanelApplet *base       = VALA_PANEL_APPLET(self);
	GSettings *settings         = vala_panel_applet_get_settings(base);
	ValaPanelToplevel *toplevel = vala_panel_applet_get_toplevel(base);
	GActionMap *map             = G_ACTION_MAP(vala_panel_applet_get_action_group(base));
	g_simple_action_set_enabled(
	    G_SIMPLE_ACTION(g_action_map_lookup_action(map, VALA_PANEL_APPLET_ACTION_CONFIGURE)),
	    true);
	g_simple_action_set_enabled(
	    G_SIMPLE_ACTION(g_action_map_lookup_action(map, VALA_PANEL_APPLET_ACTION_REMOTE)),
	    true);
	GenMonWidget *widget = genmon_widget_new();
	self->widget         = widget;
	g_settings_bind(settings, GENMON_USE_TITLE, widget, GENMON_USE_TITLE, G_SETTINGS_BIND_GET);
	g_settings_bind(settings,
	                GENMON_TITLE_TEXT,
	                widget,
	                GENMON_TITLE_TEXT,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(settings,
	                GENMON_UPDATE_PERIOD,
	                widget,
	                GENMON_UPDATE_PERIOD,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(settings, GENMON_CMD, widget, GENMON_CMD, G_SETTINGS_BIND_GET);
	g_settings_bind(settings, GENMON_FONT, widget, GENMON_FONT, G_SETTINGS_BIND_GET);

	g_object_bind_property(toplevel,
	                       VP_KEY_ORIENTATION,
	                       widget,
	                       VP_KEY_ORIENTATION,
	                       G_BINDING_SYNC_CREATE);

	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(self));
}

static GtkWidget *genmon_applet_get_settings_ui(ValaPanelApplet *base)
{
	GenMonConfig *config = genmon_config_new(); /* Configuration/option dialog */
	genmon_config_init_gsettings(config, vala_panel_applet_get_settings(base));
	gtk_widget_show(GTK_WIDGET(config));

	return GTK_WIDGET(config);
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

static void genmon_applet_update_context_menu(ValaPanelApplet *base, GMenu *parent)
{
	g_autoptr(GMenuItem) refresh_item =
	    g_menu_item_new(g_dgettext(GETTEXT_PACKAGE, "Refresh"), NULL);
	g_menu_item_set_action_and_target(refresh_item,
	                                  "applet." VALA_PANEL_APPLET_ACTION_REMOTE,
	                                  "s",
	                                  "refresh");
	g_menu_prepend_item(parent, refresh_item);
}

static void genmon_applet_init(GenMonApplet *self)
{
}

static void genmon_applet_class_init(GenMonAppletClass *klass)
{
	VALA_PANEL_APPLET_CLASS(klass)->get_settings_ui     = genmon_applet_get_settings_ui;
	VALA_PANEL_APPLET_CLASS(klass)->remote_command      = genmon_applet_remote_command;
	VALA_PANEL_APPLET_CLASS(klass)->update_context_menu = genmon_applet_update_context_menu;
	G_OBJECT_CLASS(klass)->constructed                  = genmon_applet_constructed;
}

static void genmon_applet_class_finalize(GenMonAppletClass *klass)
{
}

/*
 * IO Module functions
 */

void g_io_genmon_load(GTypeModule *module)
{
	g_return_if_fail(module != NULL);

	genmon_applet_register_type(module);

	g_io_extension_point_implement(VALA_PANEL_APPLET_EXTENSION_POINT,
	                               genmon_applet_get_type(),
	                               "org.valapanel.genmon",
	                               10);
}

void g_io_genmon_unload(GIOModule *module)
{
	g_return_if_fail(module != NULL);
}
