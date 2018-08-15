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

#include "mate-panel-genmon.h"
#include "config.h"
#include "config_gui.h"
#include "mon_widget.h"

#include <glib/gi18n.h>
#include <mate-panel-applet-gsettings.h>

#define BORDER 2

struct _GenMonApplet
{
	MatePanelApplet parent;
	GenMonWidget *widget;
	GSettings *settings;
};

G_DEFINE_DYNAMIC_TYPE(GenMonApplet, genmon_applet, mate_panel_applet_get_type())

static void genmon_applet_get_settings_ui(GtkAction *action, GenMonApplet *self);
static void genmon_applet_about(GtkAction *action, GenMonApplet *self);

static const GtkActionEntry genmon_menu_verbs[] =
    { { "GenMonPreferences",
	"document-properties",
	N_("_Preferences"),
	NULL,
	NULL,
	G_CALLBACK(genmon_applet_get_settings_ui) },
      { "GenMonAbout", "help-about", N_("_About"), NULL, NULL, G_CALLBACK(genmon_applet_about) } };

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
static void genmon_applet_update_context_menu(GenMonApplet *self)
{
	GtkActionGroup *action_group = gtk_action_group_new("GenMonApplet Menu Actions");
	gtk_action_group_set_translation_domain(action_group, GETTEXT_PACKAGE);
	gtk_action_group_add_actions(action_group,
	                             genmon_menu_verbs,
	                             G_N_ELEMENTS(genmon_menu_verbs),
	                             self);
	mate_panel_applet_setup_menu(
	    MATE_PANEL_APPLET(self),
	    "<menuitem name=\"GenMon Preferences Item\" action=\"GenMonPreferences\" />"
	    "<menuitem name=\"GenMon About Item\" action=\"GenMonAbout\" />",
	    action_group);
}
G_GNUC_END_IGNORE_DEPRECATIONS

bool genmon_applet_panel_init(GenMonApplet *self)
{
	GSettings *settings =
	    mate_panel_applet_settings_new(MATE_PANEL_APPLET(self), "org.valapanel.genmon");
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

	genmon_applet_update_context_menu(self);

	gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(widget));
	gtk_widget_show(GTK_WIDGET(self));

	return self;
}

static void genmon_applet_get_settings_ui(GtkAction *action, GenMonApplet *self)
{
	GenMonConfig *config = genmon_config_new(); /* Configuration/option dialog */

	GtkDialog *dlg = GTK_DIALOG(gtk_dialog_new());
	gtk_window_set_title(GTK_WINDOW(dlg), _("Configuration"));
	gtk_window_set_transient_for(GTK_WINDOW(dlg),
	                             GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(self))));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(dlg), true);
	genmon_config_init_gsettings(config, self->settings);

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

static void genmon_applet_about(GtkAction *action, GenMonApplet *self)
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

static void genmon_applet_change_orient(MatePanelApplet *base, MatePanelAppletOrient orientation)
{
	GenMonApplet *self = GENMON_APPLET(base);
	if (orientation < MATE_PANEL_APPLET_ORIENT_LEFT)
		gtk_orientable_set_orientation(self->widget, GTK_ORIENTATION_HORIZONTAL);
	else
		gtk_orientable_set_orientation(self->widget, GTK_ORIENTATION_VERTICAL);
}

static void genmon_applet_init(GenMonApplet *self)
{
}

static void genmon_applet_finalize(GObject *base)
{
	GenMonApplet *self = GENMON_APPLET(base);
	g_clear_pointer(&self->settings, g_object_unref);
	g_clear_pointer(&self->widget, g_object_unref);
}

static void genmon_applet_class_init(GenMonAppletClass *klass)
{
	((MatePanelAppletClass *)klass)->change_orient = genmon_applet_change_orient;
	((GObjectClass *)klass)->finalize              = genmon_applet_finalize;
}

static void genmon_applet_class_finalize(GenMonAppletClass *klass)
{
}

/*
 * Plugin functions
 */

static bool genmon_factory(MatePanelApplet *applet, const char *iid, gpointer data)
{
	bool retval = FALSE;

	if (!strcmp(iid, "GenMonApplet"))
	{
		retval = genmon_applet_panel_init(GENMON_APPLET(applet));
	}

	return retval;
}

MATE_PANEL_APPLET_IN_PROCESS_FACTORY("GenMonAppletFactory", genmon_applet_get_type(),
                                     "GenericMonitor", genmon_factory, NULL)
