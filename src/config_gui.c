/*
 *  Generic Monitor plugin
 *  Construct the configure GUI
 *  Copyright (c) 2004 Roger Seguin <roger_seguin@msn.com>
 *                                  <http://rmlx.dyndns.org>
 *  Copyright (C) 2015-2018 Konstantin Pugin <ria.freelander@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.

 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config_gui.h"

#include "config.h"

#include <glib/gi18n.h>
#include <gtk/gtk.h>

struct _GenMonConfig
{
	GtkGrid __parent__;
	/* Configuration GUI widgets */
	GtkWidget *entry_cmd;
	GtkWidget *spin_interval;
	GtkWidget *entry_title;
	GtkWidget *check_title;
	GtkWidget *fntbutton;
};

G_DEFINE_TYPE(GenMonConfig, genmon_config, GTK_TYPE_GRID)

void genmon_config_init(GenMonConfig *self)
{
	gtk_widget_init_template(GTK_WIDGET(self));
	g_object_bind_property(self->check_title,
	                       "active",
	                       self->entry_title,
	                       "sensitive",
	                       (GBindingFlags)(G_BINDING_SYNC_CREATE));
}

void genmon_config_class_init(GenMonConfigClass *klass)
{
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass),
	                                            "/org/genmon/config.ui");
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "entry-cmd",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonConfig, entry_cmd));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "spin-interval",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonConfig, spin_interval));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "fntbutton",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonConfig, fntbutton));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "entry-title",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonConfig, entry_title));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "check-title",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonConfig, check_title));
}

GenMonConfig *genmon_config_new()
{
	return GENMON_CONFIG(g_object_new(genmon_config_get_type(), NULL));
}

void genmon_config_init_gsettings(GenMonConfig *ui, GSettings *settings)
{
	g_settings_bind(settings, GENMON_CMD, ui->entry_cmd, "text", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(settings, GENMON_FONT, ui->fntbutton, "font", G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(settings,
	                GENMON_TITLE_TEXT,
	                ui->entry_title,
	                "text",
	                G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(settings,
	                GENMON_UPDATE_PERIOD,
	                ui->spin_interval,
	                "value",
	                G_SETTINGS_BIND_DEFAULT);
	g_settings_bind(settings,
	                GENMON_USE_TITLE,
	                ui->check_title,
	                "active",
	                G_SETTINGS_BIND_DEFAULT);
	gtk_widget_set_sensitive(GTK_WIDGET(ui->entry_title),
	                         gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->check_title)));
}

void genmon_config_init_properties(GenMonConfig *self, GenMonWidget *widget)
{
	g_object_bind_property(widget,
	                       GENMON_USE_TITLE,
	                       self->check_title,
	                       "active",
	                       (GBindingFlags)(G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE));
	g_object_bind_property(widget,
	                       GENMON_CMD,
	                       self->entry_cmd,
	                       "text",
	                       (GBindingFlags)(G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE));
	g_object_bind_property(widget,
	                       GENMON_FONT,
	                       self->fntbutton,
	                       "font",
	                       (GBindingFlags)(G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE));
	g_object_bind_property(widget,
	                       GENMON_TITLE_TEXT,
	                       self->entry_title,
	                       "text",
	                       (GBindingFlags)(G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE));
	g_object_bind_property(widget,
	                       GENMON_UPDATE_PERIOD,
	                       self->spin_interval,
	                       "value",
	                       (GBindingFlags)(G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE));
}
