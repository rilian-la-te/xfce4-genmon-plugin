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

#include "xfce4-panel-genmon.h"
#include "config.h"
#include "config_gui.h"
#include "mon_widget.h"

#include <glib/gi18n.h>
#include <locale.h>
#include <stdbool.h>
#include <xfconf/xfconf.h>

#define BORDER 2

struct _GenMonApplet
{
	XfcePanelPlugin parent;
	GenMonWidget *widget;
	XfconfChannel *channel;
};

G_DEFINE_DYNAMIC_TYPE(GenMonApplet, genmon_applet, XFCE_TYPE_PANEL_PLUGIN);

static void genmon_applet_mode_changed(XfcePanelPlugin *sender, XfcePanelPluginMode mode)
{
	GenMonApplet *self = GENMON_APPLET(sender);
	gtk_orientable_set_orientation(GTK_ORIENTABLE(self->widget),
	                               mode != XFCE_PANEL_PLUGIN_MODE_VERTICAL
	                                   ? GTK_ORIENTATION_HORIZONTAL
	                                   : GTK_ORIENTATION_VERTICAL);
}

void genmon_applet_construct(XfcePanelPlugin *parent)
{
	GenMonApplet *self = GENMON_APPLET(parent);
	setlocale(LC_CTYPE, "");
	bindtextdomain(GETTEXT_PACKAGE, LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	GenMonWidget *widget = genmon_widget_new();
	self->widget         = widget;
	gtk_container_add(GTK_CONTAINER(parent), GTK_WIDGET(self));
	xfce_panel_plugin_add_action_widget(parent, GTK_WIDGET(widget));
	gtk_orientable_set_orientation(GTK_ORIENTABLE(self->widget),
	                               xfce_panel_plugin_get_mode(parent) ==
	                                       XFCE_PANEL_PLUGIN_MODE_VERTICAL
	                                   ? GTK_ORIENTATION_VERTICAL
	                                   : GTK_ORIENTATION_HORIZONTAL);

	xfconf_init(NULL);

	self->channel = xfce_panel_plugin_xfconf_channel_new(parent);
	char *str =
	    g_strdup_printf("%s/%s", xfce_panel_plugin_get_property_base(parent), GENMON_PROP_CMD);
	xfconf_g_property_bind(self->channel, str, G_TYPE_STRING, widget, GENMON_PROP_CMD);
	g_free(str);
	str = g_strdup_printf("%s/%s",
	                      xfce_panel_plugin_get_property_base(parent),
	                      GENMON_PROP_TITLE_TEXT);
	xfconf_g_property_bind(self->channel, str, G_TYPE_STRING, widget, GENMON_PROP_TITLE_TEXT);
	g_free(str);
	str = g_strdup_printf("%s/%s",
	                      xfce_panel_plugin_get_property_base(parent),
	                      GENMON_PROP_UPDATE_PERIOD);
	xfconf_g_property_bind(self->channel, str, G_TYPE_UINT, widget, GENMON_PROP_UPDATE_PERIOD);
	g_free(str);
	str =
	    g_strdup_printf("%s/%s", xfce_panel_plugin_get_property_base(parent), GENMON_PROP_FONT);
	xfconf_g_property_bind(self->channel, str, G_TYPE_STRING, widget, GENMON_PROP_FONT);
	g_free(str);
	str = g_strdup_printf("%s/%s",
	                      xfce_panel_plugin_get_property_base(parent),
	                      GENMON_PROP_USE_TITLE);
	xfconf_g_property_bind(self->channel, str, G_TYPE_BOOLEAN, widget, GENMON_PROP_USE_TITLE);
	g_free(str);

	gtk_widget_show_all(GTK_WIDGET(self));
	xfce_panel_plugin_set_shrink(parent, true);
	return;
}

static void genmon_applet_configure_plugin(XfcePanelPlugin *base)
{
	GenMonApplet *self = GENMON_APPLET(base);
	xfce_panel_plugin_block_menu(base);
	GenMonConfig *config = genmon_config_new(); /* Configuration/option dialog */

	GtkDialog *dlg = GTK_DIALOG(gtk_dialog_new());
	gtk_window_set_title(GTK_WINDOW(dlg), _("Configuration"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg),
	                             GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(base))));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(dlg), true);
	genmon_config_init_properties(config, self->widget);

	g_signal_connect(dlg, "unmap", G_CALLBACK(gtk_widget_destroy), dlg);
	gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dlg))),
	                   GTK_WIDGET(config),
	                   true,
	                   true,
	                   0);

	gtk_widget_show_all(GTK_WIDGET(config));

	gtk_window_present(GTK_WINDOW(dlg));

	return;
}

static void genmon_applet_about(XfcePanelPlugin *plugin)
{
	const gchar *auth[] = { "Roger Seguin <roger_seguin@msn.com>",
		                "Julien Devemy <jujucece@gmail.com>",
		                "Tony Paulic <tony.paulic@gmail.com>",
		                NULL };

	gtk_show_about_dialog(NULL,
	                      "logo-icon-name",
	                      "utilities-system-monitor",
	                      "license-type",
	                      GTK_LICENSE_LGPL_3_0,
	                      "version",
	                      VERSION,
	                      "program-name",
	                      GETTEXT_PACKAGE,
	                      "comments",
	                      _("Cyclically spawns a script/program, captures its output and "
	                        "displays the resulting string in the panel"),
	                      "website",
	                      "http://goodies.xfce.org/projects/panel-plugins/xfce4-genmon-plugin",
	                      "copyright",
	                      _("Copyright \xc2\xa9 2004 Roger Seguin\nCopyright \xc2\xa9 2006 "
	                        "Julien Devemy\nCopyright \xc2\xa9 2016 Tony Paulic\n"),
	                      "authors",
	                      auth,
	                      NULL);
}

static gboolean genmon_set_size(XfcePanelPlugin *plugin, int size)
/* Plugin API */
/* Set the size of the panel-docked monitor */
{
	GenMonApplet *self = GENMON_APPLET(plugin);

	if (xfce_panel_plugin_get_orientation(plugin) == GTK_ORIENTATION_HORIZONTAL)
	{
		if (size > BORDER)
			gtk_widget_set_size_request(GTK_WIDGET(self->widget), 8, size - BORDER * 2);
	}
	else
	{
		if (size > BORDER)
			gtk_widget_set_size_request(GTK_WIDGET(self->widget), size - BORDER * 2, 8);
	}

	return TRUE;
} /* genmon_set_size() */

static gboolean genmon_applet_remote_event(XfcePanelPlugin *base, const char *command,
                                           const GValue *value)
{
	GenMonApplet *self = GENMON_APPLET(base);
	g_return_val_if_fail(value == NULL || G_IS_VALUE(value), FALSE);
	if (strcmp(command, "refresh") == 0)
	{
		if (value != NULL && G_VALUE_HOLDS_BOOLEAN(value) && g_value_get_boolean(value))
		{
			/* update the display */
			genmon_widget_display_command_output(self->widget);
		}
		return true;
	}
	return false;
}

static void genmon_applet_init(GenMonApplet *self)
{
}

static void genmon_applet_class_init(GenMonAppletClass *klass)
{
	((XfcePanelPluginClass *)klass)->construct        = genmon_applet_construct;
	((XfcePanelPluginClass *)klass)->configure_plugin = genmon_applet_configure_plugin;
	((XfcePanelPluginClass *)klass)->mode_changed     = genmon_applet_mode_changed;
	((XfcePanelPluginClass *)klass)->remote_event     = genmon_applet_remote_event;
	((XfcePanelPluginClass *)klass)->size_changed     = genmon_set_size;
	((XfcePanelPluginClass *)klass)->about            = genmon_applet_about;
}

static void genmon_applet_class_finalize(GenMonAppletClass *klass)
{
}

GType xfce_panel_module_init(GTypeModule *module)
{
	genmon_applet_register_type(module);

	g_type_module_use(module);
	return genmon_applet_get_type();
}
