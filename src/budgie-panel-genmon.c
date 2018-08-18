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

#include "budgie-panel-genmon.h"
#include "config_gui.h"
#include "mon_widget.h"

#include <stdbool.h>

#define BORDER 2

enum
{
	PROP_ZERO,
	PROP_UUID,
	LAST_PROP = PROP_UUID
};

static GParamSpec *pspec;

struct _GenMonApplet
{
	BudgieApplet parent;
	GenMonWidget *widget;
	GSettings *settings;
	char *uuid;
};

G_DEFINE_DYNAMIC_TYPE(GenMonApplet, genmon_applet, budgie_applet_get_type());

static GObject *genmon_applet_constructor(GType type, guint n_construct_properties,
                                          GObjectConstructParam *construct_properties)
{
	GObjectClass *parent_class = G_OBJECT_CLASS(genmon_applet_parent_class);
	GObject *obj =
	    parent_class->constructor(type, n_construct_properties, construct_properties);
	GenMonApplet *self   = GENMON_APPLET(obj);
	GenMonWidget *widget = genmon_widget_new();
	self->widget         = GENMON_WIDGET(widget);
	budgie_applet_set_settings_schema(BUDGIE_APPLET(self), "org.valapanel.genmon");
	budgie_applet_set_settings_prefix(BUDGIE_APPLET(self),
	                                  "/com/solus-project/budgie-panel/instance/genmon");
	self->settings = budgie_applet_get_applet_settings(BUDGIE_APPLET(self), self->uuid);
	g_settings_bind(self->settings,
	                GENMON_PROP_USE_TITLE,
	                widget,
	                GENMON_PROP_USE_TITLE,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(self->settings,
	                GENMON_PROP_TITLE_TEXT,
	                widget,
	                GENMON_PROP_TITLE_TEXT,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(self->settings,
	                GENMON_PROP_UPDATE_PERIOD,
	                widget,
	                GENMON_PROP_UPDATE_PERIOD,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(self->settings,
	                GENMON_PROP_CMD,
	                widget,
	                GENMON_PROP_CMD,
	                G_SETTINGS_BIND_GET);
	g_settings_bind(self->settings,
	                GENMON_PROP_FONT,
	                widget,
	                GENMON_PROP_FONT,
	                G_SETTINGS_BIND_GET);

	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(self));
	return obj;
}

GenMonApplet *genmon_applet_new(const char *uuid)
{
	return GENMON_APPLET(g_object_new(genmon_applet_get_type(), "uuid", uuid, NULL));
}

static void genmon_applet_panel_position_changed(BudgieApplet *base, BudgiePanelPosition pos)
{
	GenMonApplet *self = GENMON_APPLET(base);
	if (pos < BUDGIE_PANEL_POSITION_LEFT)
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->widget),
		                               GTK_ORIENTATION_HORIZONTAL);
	else
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->widget),
		                               GTK_ORIENTATION_VERTICAL);
}

static GtkWidget *genmon_applet_get_settings_ui(BudgieApplet *base)
{
	GenMonApplet *self   = GENMON_APPLET(base);
	GenMonConfig *config = genmon_config_new(); /* Configuration/option dialog */
	genmon_config_init_gsettings(config, self->settings);
	gtk_widget_show_all(GTK_WIDGET(config));

	return GTK_WIDGET(g_object_ref_sink(config));
}

static gboolean genmon_applet_supports_settings(BudgieApplet *base)
{
	return true;
}

static void genmon_applet_set_property(GObject *object, uint prop_id, const GValue *value,
                                       GParamSpec *pspec)
{
	GenMonApplet *self = GENMON_APPLET(object);
	switch (prop_id)
	{
	case PROP_UUID:
		g_clear_pointer(&self->uuid, g_free);
		self->uuid = g_value_dup_string(value);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void genmon_applet_get_property(GObject *object, uint prop_id, GValue *value,
                                       GParamSpec *pspec)
{
	GenMonApplet *self = GENMON_APPLET(object);
	switch (prop_id)
	{
	case PROP_UUID:
		g_value_set_string(value, self->uuid);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void genmon_applet_init(GenMonApplet *self)
{
}

static void genmon_applet_destroy(GtkWidget *object)
{
	GenMonApplet *self = GENMON_APPLET(object);
	if (self->widget)
	{
		g_settings_unbind(self->widget, GENMON_PROP_USE_TITLE);
		g_settings_unbind(self->widget, GENMON_PROP_TITLE_TEXT);
		g_settings_unbind(self->widget, GENMON_PROP_CMD);
		g_settings_unbind(self->widget, GENMON_PROP_UPDATE_PERIOD);
		g_settings_unbind(self->widget, GENMON_PROP_FONT);
	}
	g_clear_pointer(&self->widget, gtk_widget_destroy);
}

static void genmon_applet_finalize(GObject *object)
{
	GenMonApplet *self = GENMON_APPLET(object);
	g_clear_pointer(&self->uuid, g_free);
	g_clear_object(&self->settings);
}

static void genmon_applet_class_init(GenMonAppletClass *klass)
{
	((GObjectClass *)klass)->constructor                 = genmon_applet_constructor;
	((GObjectClass *)klass)->get_property                = genmon_applet_get_property;
	((GObjectClass *)klass)->set_property                = genmon_applet_set_property;
	((GObjectClass *)klass)->finalize                    = genmon_applet_finalize;
	((GtkWidgetClass *)klass)->destroy                   = genmon_applet_destroy;
	((BudgieAppletClass *)klass)->get_settings_ui        = genmon_applet_get_settings_ui;
	((BudgieAppletClass *)klass)->panel_position_changed = genmon_applet_panel_position_changed;
	((BudgieAppletClass *)klass)->supports_settings      = genmon_applet_supports_settings;
	pspec                                                = g_param_spec_string("uuid",
                                    "uuid",
                                    "uuid",
                                    "",
                                    (GParamFlags)(G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                                                  G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
                                                  G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(((GObjectClass *)klass), PROP_UUID, pspec);
}

static void genmon_applet_class_finalize(GenMonAppletClass *klass)
{
}

/*
 * Plugin functions
 */

struct _GenMonPlugin
{
	PeasExtensionBase parent;
};

static void budgie_plugin_interface_init(BudgiePluginIface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED(GenMonPlugin, genmon_plugin, peas_extension_base_get_type(), 0,
                               G_IMPLEMENT_INTERFACE_DYNAMIC(budgie_plugin_get_type(),
                                                             budgie_plugin_interface_init));

static BudgieApplet *genmon_plugin_get_panel_widget(BudgiePlugin *base, char *uuid)
{
	g_return_val_if_fail(uuid != NULL, NULL);

	return BUDGIE_APPLET(g_object_ref_sink(genmon_applet_new(uuid)));
}

GenMonPlugin *genmon_plugin_new(GType object_type)
{
	return GENMON_PLUGIN(g_object_new(genmon_plugin_get_type(), NULL));
}

static void genmon_plugin_class_init(GenMonPluginClass *klass)
{
}

static void budgie_plugin_interface_init(BudgiePluginIface *iface)
{
	iface->get_panel_widget = genmon_plugin_get_panel_widget;
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
void peas_register_types(GTypeModule *module)
{
	g_return_if_fail(module != NULL);

	PeasObjectModule *objmodule = PEAS_OBJECT_MODULE(module);

	genmon_applet_register_type(module);
	genmon_plugin_register_type(module);

	peas_object_module_register_extension_type(objmodule,
	                                           budgie_plugin_get_type(),
	                                           genmon_plugin_get_type());
}
