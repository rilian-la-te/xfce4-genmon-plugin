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

/* Code borrowed from vala-panel to avoid dependency on it */
#include <stdbool.h>

#include "util.h"

void css_apply_from_file_with_class(GtkWidget *widget, const char *file, const char *klass,
                                    bool remove)
{
	g_autoptr(GError) error  = NULL;
	GtkStyleContext *context = gtk_widget_get_style_context(widget);
	gtk_widget_reset_style(widget);
	if (remove)
	{
		gtk_style_context_remove_class(context, klass);
	}
	else
	{
		g_autoptr(GtkCssProvider) provider = gtk_css_provider_new();
		gtk_css_provider_load_from_path(provider, file, &error);
		if (error)
		{
			g_warning("%s\n", error->message);
			return;
		}
		gtk_style_context_add_provider(context,
		                               GTK_STYLE_PROVIDER(provider),
		                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 2);
		if (klass)
			gtk_style_context_add_class(context, klass);
	}
}

void css_apply_with_class(GtkWidget *widget, const char *css, const char *klass, bool remove)
{
	GtkStyleContext *context = gtk_widget_get_style_context(widget);
	gtk_widget_reset_style(widget);
	if (remove)
	{
		gtk_style_context_remove_class(context, klass);
	}
	else
	{
		g_autoptr(GtkCssProvider) provider = gtk_css_provider_new();
		gtk_css_provider_load_from_data(provider, css, -1, NULL);
		gtk_style_context_add_provider(context,
		                               GTK_STYLE_PROVIDER(provider),
		                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 1);
		if (klass)
			gtk_style_context_add_class(context, klass);
	}
}

static void set_widget_align(GtkWidget *user_data, gpointer data)
{
	if (GTK_IS_WIDGET(user_data))
	{
		gtk_widget_set_halign(GTK_WIDGET(user_data), GTK_ALIGN_FILL);
		gtk_widget_set_valign(GTK_WIDGET(user_data), GTK_ALIGN_FILL);
	}
}

/* Children hierarhy: button => alignment => box => (label,image) */
static void setup_button_notify_connect(GObject *_sender, GParamSpec *b, gpointer self)
{
	GtkButton *a = GTK_BUTTON(_sender);
	if (!strcmp(b->name, "label") || !strcmp(b->name, "image"))
	{
		GtkWidget *w = gtk_bin_get_child(GTK_BIN(a));
		if (GTK_IS_CONTAINER(w))
		{
			GtkWidget *ch;
			if (GTK_IS_BIN(w))
				ch = gtk_bin_get_child(GTK_BIN(w));
			else
				ch = w;
			if (GTK_IS_CONTAINER(ch))
				gtk_container_forall(GTK_CONTAINER(ch), set_widget_align, NULL);
			gtk_widget_set_halign(ch, GTK_ALIGN_FILL);
			gtk_widget_set_valign(ch, GTK_ALIGN_FILL);
		}
	}
}

void vala_panel_setup_button(GtkButton *b, GtkImage *img, const char *label)
{
	const char *css =
	    ".-panel-button"
	    "{"
	    "color: inherit;"
	    "margin: 0px 0px 0px 0px;"
	    "padding: 0px 0px 0px 0px;"
	    "}";
	css_apply_with_class(GTK_WIDGET(b), css, "-panel-button", false);
	g_signal_connect(b, "notify", G_CALLBACK(setup_button_notify_connect), NULL);
	if (img != NULL)
	{
		gtk_button_set_image(b, GTK_WIDGET(img));
		gtk_button_set_always_show_image(b, true);
	}
	if (label != NULL)
		gtk_button_set_label(b, label);
	gtk_button_set_relief(b, GTK_RELIEF_NONE);
}

static void child_spawn_func(void *data)
{
	setpgid(0, getpgid(getppid()));
}

bool vala_panel_launch_with_context(GDesktopAppInfo *app_info, GAppLaunchContext *cxt, GList *uris)
{
	g_autoptr(GError) err = NULL;
	if (app_info == NULL)
		return false;
	bool ret = g_desktop_app_info_launch_uris_as_manager(G_DESKTOP_APP_INFO(app_info),
	                                                     uris,
	                                                     cxt,
	                                                     G_SPAWN_SEARCH_PATH,
	                                                     child_spawn_func,
	                                                     NULL,
	                                                     NULL,
	                                                     NULL,
	                                                     &err);
	if (err)
		g_warning("%s\n", err->message);
	return ret;
}
