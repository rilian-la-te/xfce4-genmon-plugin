/*
 *  Generic Monitor plugin for the Xfce4 panel
 *  Construct the configure GUI
 *  Copyright (c) 2004 Roger Seguin <roger_seguin@msn.com>
 *                                  <http://rmlx.dyndns.org>
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

#define COPYVAL(var, field) ((var)->field = field)

/**** GUI initially created using glade-2 ****/

/* Use the gtk_button_new_with_mnemonic() function for text-based
   push buttons */
/* Use "#define gtk_button_new_with_mnemonic(x) gtk_button_new()"
   for color-filled buttons */

#define gtk_button_new_with_mnemonic(x) gtk_button_new()

#define GLADE_HOOKUP_OBJECT(component, widget, name)                                               \
	g_object_set_data_full(G_OBJECT(component),                                                \
	                       name,                                                               \
	                       gtk_widget_ref(widget),                                             \
	                       (GDestroyNotify)gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component, widget, name)                                        \
	g_object_set_data(G_OBJECT(component), name, widget)

int genmon_CreateConfigGUI(GtkWidget *vbox1, struct gui_t *p_poGUI)
{
	GtkWidget *table1;
	GtkWidget *label1;
	GtkWidget *wTF_Cmd;
	GtkWidget *eventbox1;
	GtkAdjustment *wSc_Period_adj;
	GtkWidget *wSc_Period;
	GtkWidget *label2;
	GtkWidget *wTB_Title;
	GtkWidget *wTF_Title;
	GtkWidget *hseparator10;
	GtkWidget *wPB_Font;
	GtkWidget *hbox4;

	table1 = gtk_grid_new();
	gtk_grid_set_column_spacing(GTK_GRID(table1), 2);
	gtk_grid_set_row_spacing(GTK_GRID(table1), 2);
	gtk_widget_show(table1);
	gtk_box_pack_start(GTK_BOX(vbox1), table1, FALSE, TRUE, 0);

	label1 = gtk_label_new(g_dgettext(GETTEXT_PACKAGE, "Command"));
	gtk_widget_show(label1);
	gtk_grid_attach(GTK_GRID(table1), label1, 0, 0, 1, 1);
	//(GtkAttachOptions) (GTK_FILL),
	//(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label1), GTK_JUSTIFY_LEFT);
	gtk_widget_set_valign(label1, GTK_ALIGN_CENTER);

	wTF_Cmd = gtk_entry_new();
	gtk_widget_show(wTF_Cmd);
	gtk_grid_attach(GTK_GRID(table1), wTF_Cmd, 1, 0, 1, 1);
	gtk_widget_set_tooltip_text(
	    wTF_Cmd,
	    g_dgettext(GETTEXT_PACKAGE, "Input the shell command to spawn, then press <Enter>"));
	gtk_entry_set_max_length(GTK_ENTRY(wTF_Cmd), 128);

	eventbox1 = gtk_event_box_new();
	gtk_widget_show(eventbox1);
	gtk_grid_attach(GTK_GRID(table1), eventbox1, 1, 2, 1, 1);
	gtk_widget_set_valign(GTK_WIDGET(eventbox1), GTK_ALIGN_CENTER);
	gtk_widget_set_halign(GTK_WIDGET(eventbox1), GTK_ALIGN_CENTER);
	gtk_widget_set_vexpand(GTK_WIDGET(eventbox1), TRUE);
	gtk_widget_set_hexpand(GTK_WIDGET(eventbox1), TRUE);

	wSc_Period_adj = gtk_adjustment_new(15000, 200, 1000 * 60 * 60 * 24, 50, 1000, 0);
	wSc_Period     = gtk_spin_button_new(GTK_ADJUSTMENT(wSc_Period_adj), 50, 2);
	gtk_widget_show(wSc_Period);
	gtk_container_add(GTK_CONTAINER(eventbox1), wSc_Period);
	gtk_widget_set_tooltip_text(wSc_Period,
	                            g_dgettext(GETTEXT_PACKAGE,
	                                       "Interval between 2 consecutive spawns"));
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(wSc_Period), TRUE);

	label2 = gtk_label_new(g_dgettext(GETTEXT_PACKAGE, "Period (ms) "));
	gtk_widget_show(label2);
	gtk_grid_attach(GTK_GRID(table1), label2, 0, 2, 1, 1);
	//(GtkAttachOptions) (GTK_FILL),
	//(GtkAttachOptions) (0), 0, 0);
	gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_LEFT);
	gtk_widget_set_valign(label2, GTK_ALIGN_CENTER);

	wTB_Title = gtk_check_button_new_with_mnemonic(g_dgettext(GETTEXT_PACKAGE, "Label"));
	gtk_widget_show(wTB_Title);
	gtk_grid_attach(GTK_GRID(table1), wTB_Title, 0, 1, 1, 1);
	gtk_widget_set_tooltip_text(wTB_Title,
	                            g_dgettext(GETTEXT_PACKAGE, "Tick to display label"));

	wTF_Title = gtk_entry_new();
	gtk_widget_show(wTF_Title);
	gtk_grid_attach(GTK_GRID(table1), wTF_Title, 1, 1, 1, 1);

	gtk_widget_set_tooltip_text(wTF_Title,
	                            g_dgettext(GETTEXT_PACKAGE,
	                                       "Input the plugin label, then press <Enter>"));
	gtk_entry_set_max_length(GTK_ENTRY(wTF_Title), 16);
	gtk_entry_set_text(GTK_ENTRY(wTF_Title), g_dgettext(GETTEXT_PACKAGE, "(genmon)"));

	hseparator10 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_show(hseparator10);
	gtk_box_pack_start(GTK_BOX(vbox1), hseparator10, FALSE, FALSE, 0);

	wPB_Font = gtk_font_button_new();
	gtk_widget_show(wPB_Font);
	gtk_box_pack_start(GTK_BOX(vbox1), wPB_Font, TRUE, TRUE, 0);
	gtk_widget_set_tooltip_text(wPB_Font,
	                            g_dgettext(GETTEXT_PACKAGE,
	                                       "Press to change font for plugin..."));

	hbox4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	gtk_widget_show(hbox4);
	gtk_container_add(GTK_CONTAINER(vbox1), hbox4);

	if (p_poGUI)
	{
		COPYVAL(p_poGUI, wTF_Cmd);
		COPYVAL(p_poGUI, wTB_Title);
		COPYVAL(p_poGUI, wTF_Title);
		COPYVAL(p_poGUI, wSc_Period);
		COPYVAL(p_poGUI, wPB_Font);
	}
	return (0);
} /* CreateConfigGUI() */
