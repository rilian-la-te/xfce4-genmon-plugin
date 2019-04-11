/*
 * generic-monitor-plugin
 * Copyright (c) 2004 Roger Seguin <roger_seguin@msn.com>
 *                                 <http://rmlx.dyndns.org>
 * Copyright (c) 2006 Julien Devemy <jujucece@gmail.com>
 * Copyright (c) 2012 John Lindgren <john.lindgren@aol.com>
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

#include "mon_widget.h"

#include <glib/gi18n.h>
#include <stdbool.h>
#include <stdint.h>

#include "config.h"

#include "cmdspawn.h"
#include "config_gui.h"
#include "util.h"

enum
{
	PROP_0,
	PROP_COMMAND,
	PROP_TITLE,
	PROP_IS_TITLE_DISPAYED,
	PROP_UPDATE_INTERVAL_MS,
	PROP_FONT_VALUE,

	/* orientable */
	PROP_ORIENTATION,
	LAST_PROP = PROP_ORIENTATION
};

static GParamSpec *pspecs[LAST_PROP];

struct _GenMonWidget
{
	GtkEventBox __parent__;
	unsigned int timer_id; /* Cyclic update */
	                       /* Plugin monitor */
	GtkBox *main_box;
	GtkBox *image_box;
	GtkLabel *title_label;
	GtkLabel *value_label;
	GtkButton *value_button;
	GtkLabel *value_button_label;
	GtkImage *image;
	GtkProgressBar *progress;
	GtkLevelBar *level;
	GtkButton *button;
	GtkImage *button_image;
	char *click_command;
	char *value_click_command;
	char *cmd_result; /* Commandline resulting string */

	/* Configurable parameters */
	char *command; /* Commandline to spawn */
	bool is_title_displayed;
	char *title;
	uint32_t update_interval_ms;
	char *font_value;
};

static void gtk_orientable_interface_init(GtkOrientableIface *iface);

G_DEFINE_TYPE_WITH_CODE(GenMonWidget, genmon_widget, GTK_TYPE_EVENT_BOX,
                        G_IMPLEMENT_INTERFACE(GTK_TYPE_ORIENTABLE, gtk_orientable_interface_init))

static void gtk_orientable_interface_init(GtkOrientableIface *iface)
{
}

/**************************************************************/

static void genmon_widget_exec_with_error_dialog(GenMonWidget *self, const char *cmd)
{
	bool run = genmon_launch_command_on_screen(cmd, gtk_widget_get_parent(GTK_WIDGET(self)));
	if (!run)
	{
		g_autofree char *first =
		    g_strdup_printf(g_dgettext(GETTEXT_PACKAGE, "Could not run \"%s\""), cmd);
		GtkDialog *dlg = GTK_DIALOG(gtk_message_dialog_new(NULL,
		                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
		                                                   GTK_MESSAGE_ERROR,
		                                                   GTK_BUTTONS_CLOSE,
		                                                   "%s",
		                                                   first));
		gtk_window_present(GTK_WINDOW(dlg));
		g_signal_connect(dlg, "response", gtk_widget_destroy, NULL);
	}
}

static void genmon_widget_exec_on_click_cmd(GtkWidget *unused, void *data)
/* Execute the onClick Command */
{
	GenMonWidget *self = GENMON_WIDGET(data);
	genmon_widget_exec_with_error_dialog(self, self->click_command);
}

static void genmon_widget_exec_on_val_click_cmd(GtkWidget *unused, void *data)
/* Execute the onClick Command */
{
	GenMonWidget *self = GENMON_WIDGET(data);
	genmon_widget_exec_with_error_dialog(self, self->value_click_command);
}

/**************************************************************/

void genmon_widget_display_command_output(GenMonWidget *self)
/* Launch the command, get its output and display it in the panel-docked
   text field */
{
	g_return_if_fail(GENMON_IS_WIDGET(self));
	char *begin;
	char *end;
	bool newVersion = false;
	g_clear_pointer(&self->cmd_result, g_free);
	self->cmd_result =
	    self->command[0] > 0 ? genmon_spawn_with_error_window(self->command, 1) : NULL;

	/* If the command fails, display XXX */
	if (!self->cmd_result)
		self->cmd_result = g_strdup("XXX");

	/* Test if the result is an Image */
	begin = strstr(self->cmd_result, "<img>");
	end   = strstr(self->cmd_result, "</img>");
	if (begin && end && begin < end)
	{
		/* Get the image path */
		char *buf = g_strndup(begin + 5, end - begin - 5);
		gtk_image_set_from_file(GTK_IMAGE(self->image), buf);
		gtk_image_set_from_file(GTK_IMAGE(self->button_image), buf);
		gtk_widget_show(GTK_WIDGET(self->button_image));
		g_free(buf);

		/* Test if the result has a clickable Image (button) */
		begin = strstr(self->cmd_result, "<click>");
		end   = strstr(self->cmd_result, "</click>");
		if (begin && end && begin < end)
		{
			/* Get the command path */
			g_free(self->click_command);
			self->click_command = g_strndup(begin + 7, end - begin - 7);

			gtk_widget_show(GTK_WIDGET(self->button));
			gtk_widget_hide(GTK_WIDGET(self->image));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(self->button));
			gtk_widget_show(GTK_WIDGET(self->image));
		}
		newVersion = true;
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(self->button));
		gtk_widget_hide(GTK_WIDGET(self->image));
	}

	/* Test if the result is a Text */
	begin = strstr(self->cmd_result, "<txt>");
	end   = strstr(self->cmd_result, "</txt>");
	if (begin && end && begin < end)
	{
		/* Get the text */
		g_autofree char *buf = g_strndup(begin + 5, end - begin - 5);
		gtk_label_set_markup(GTK_LABEL(self->value_label), buf);
		gtk_widget_show(GTK_WIDGET(self->value_button_label));

		/* Test if the result has a clickable Value (button) */
		begin = strstr(self->cmd_result, "<txtclick>");
		end   = strstr(self->cmd_result, "</txtclick>");
		if (begin && end && begin < end)
		{
			/* Add the text to the button label too*/
			gtk_label_set_markup(GTK_LABEL(self->value_button_label), buf);

			/* Get the command path */
			g_free(self->value_click_command);
			self->value_click_command = g_strndup(begin + 10, end - begin - 10);

			gtk_widget_show(GTK_WIDGET(self->value_button));
			gtk_widget_hide(GTK_WIDGET(self->value_label));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(self->value_button));
			gtk_widget_show(GTK_WIDGET(self->value_label));
		}

		newVersion = true;
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(self->value_button));
		gtk_widget_hide(GTK_WIDGET(self->value_label));
	}

	/* Test if the result is a Bar */
	begin = strstr(self->cmd_result, "<bar>");
	end   = strstr(self->cmd_result, "</bar>");
	if (begin && end && begin < end)
	{
		/* Get the text */
		g_autofree char *buf = g_strndup(begin + 5, end - begin - 5);
		int value            = atoi(buf);
		value                = (value < 0) ? 0 : (value > 100) ? 100 : value;
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(self->progress),
		                              (float)value / 100.0);
		gtk_widget_show(GTK_WIDGET(self->progress));

		newVersion = true;
	}
	else
		gtk_widget_hide(GTK_WIDGET(self->progress));

	begin = strstr(self->cmd_result, "<level>");
	end   = strstr(self->cmd_result, "</level>");
	if (begin && end && begin < end)
	{
		/* Get the text */
		g_autofree char *buf = g_strndup(begin + 7, end - begin - 7);
		int value            = atoi(buf);
		value                = (value < 0) ? 0 : (value > 100) ? 100 : value;
		gtk_level_bar_add_offset_value(self->level, GTK_LEVEL_BAR_OFFSET_LOW, 30);
		gtk_level_bar_add_offset_value(self->level, GTK_LEVEL_BAR_OFFSET_HIGH, 50);
		gtk_level_bar_add_offset_value(self->level, GTK_LEVEL_BAR_OFFSET_FULL, 80);
		gtk_level_bar_set_value(self->level, (double)value);
		gtk_widget_show(GTK_WIDGET(self->level));

		newVersion = true;
	}
	else
		gtk_widget_hide(GTK_WIDGET(self->level));

	if (!newVersion)
	{
		gtk_widget_show(GTK_WIDGET(self->value_label));
		gtk_label_set_text(GTK_LABEL(self->value_label), self->cmd_result);
	}

	/* Test if a ToolTip is given */
	begin = strstr(self->cmd_result, "<tool>");
	end   = strstr(self->cmd_result, "</tool>");
	g_autofree char *acToolTips =
	    (begin && end && begin < end)
	        ? g_strndup(begin + 6, end - begin - 6)
	        : g_strdup_printf(g_dgettext(GETTEXT_PACKAGE,
	                                     "%s\n"
	                                     "----------------\n"
	                                     "%s\n"
	                                     "Period (s): %1.3lf"),
	                          self->title,
	                          self->command,
	                          self->update_interval_ms / (double)1000);

	gtk_widget_set_tooltip_markup(GTK_WIDGET(self), acToolTips);

} /* DisplayCmdOutput() */

/* Recurrently update the panel-docked monitor through a timer */
/* Warning : should not be called directly (except the 1st time) */
/* To avoid multiple timers */
static int genmon_widget_set_timer(void *data)
{
	GenMonWidget *self = GENMON_WIDGET(data);
	genmon_widget_display_command_output(self);

	if (self->timer_id == 0)
	{
		self->timer_id = g_timeout_add(self->update_interval_ms,
		                               (GSourceFunc)genmon_widget_set_timer,
		                               self);
		return G_SOURCE_REMOVE;
	}

	return G_SOURCE_CONTINUE;
} /* SetTimer() */

/* Create the plugin widgets*/
static void genmon_widget_build(GenMonWidget *self)
{
	GtkOrientation orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(self));

	self->timer_id = 0;
	gtk_event_box_set_visible_window(GTK_EVENT_BOX(self), false);

	gtk_widget_show(GTK_WIDGET(self->main_box));
	g_object_bind_property(self,
	                       GENMON_PROP_TITLE_TEXT,
	                       self->title_label,
	                       "label",
	                       (GBindingFlags)(G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE));

	g_object_bind_property(self,
	                       GENMON_PROP_USE_TITLE,
	                       self->title_label,
	                       "visible",
	                       (GBindingFlags)(G_BINDING_BIDIRECTIONAL | G_BINDING_SYNC_CREATE));

	//	xfce_panel_plugin_add_action_widget(plugin, self->button);

	/* Setup Buttons*/
	vala_panel_setup_button(self->button, self->button_image, "");
	vala_panel_setup_button(self->value_button, NULL, NULL);

	/* Setup bars */

	if (orientation == GTK_ORIENTATION_HORIZONTAL)
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->progress),
		                               GTK_ORIENTATION_VERTICAL);
		gtk_progress_bar_set_inverted(self->progress, true);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->level),
		                               GTK_ORIENTATION_VERTICAL);
		gtk_level_bar_set_inverted(self->level, true);
	}
	else
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->progress),
		                               GTK_ORIENTATION_HORIZONTAL);
		gtk_progress_bar_set_inverted(self->progress, false);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->level),
		                               GTK_ORIENTATION_HORIZONTAL);
		gtk_level_bar_set_inverted(self->level, false);
	}

	/* make widget padding consistent */
	const char *css =
	    "\
            progressbar.horizontal trough { min-height: 6px; }\
            progressbar.horizontal progress { min-height: 6px; }\
            progressbar.vertical trough { min-width: 6px; }\
            progressbar.vertical progress { min-width: 6px; }";
	css_apply_with_class(GTK_WIDGET(self->progress), css, "", false);
	css =
	    " levelbar block { min-width: 1px; min-height: 5px; } \
            levelbar.vertical block { min-width: 5px; min-height: 1px; }\
            levelbar trough { border: none; padding: 0px; border-radius: 0px; background-color: rgba(0, 0, 0, 0); }";
	css_apply_with_class(GTK_WIDGET(self->level), css, "", false);
}

static int genmon_widget_set_font_value(GenMonWidget *poPlugin)
{
	char *css                  = NULL;
	PangoFontDescription *font = pango_font_description_from_string(poPlugin->font_value);
	if (G_LIKELY(font))
	{
		css = g_strdup_printf(
		    ".-genmon-widget-private { font-family: %s; font-size: %dpt; font-style: %s; "
		    "font-weight: %s }",
		    pango_font_description_get_family(font),
		    pango_font_description_get_size(font) / PANGO_SCALE,
		    (pango_font_description_get_style(font) == PANGO_STYLE_ITALIC ||
		     pango_font_description_get_style(font) == PANGO_STYLE_OBLIQUE)
		        ? "italic"
		        : "normal",
		    (pango_font_description_get_weight(font) >= PANGO_WEIGHT_BOLD) ? "bold"
		                                                                   : "normal");
		pango_font_description_free(font);
	}
	else
		css =
		    g_strdup_printf(".-genmon-widget-private { font: %s; }", poPlugin->font_value);

	/* Setup Gtk style */
	bool disable = false;
	if (poPlugin->font_value == NULL || !g_strcmp0(poPlugin->font_value, ""))
		disable = true;

	css_apply_with_class(GTK_WIDGET(poPlugin->title_label),
	                     css,
	                     "-genmon-widget-private",
	                     disable);
	css_apply_with_class(GTK_WIDGET(poPlugin->value_label),
	                     css,
	                     "-genmon-widget-private",
	                     disable);
	css_apply_with_class(GTK_WIDGET(poPlugin->value_button_label),
	                     css,
	                     "-genmon-widget-private",
	                     disable);

	g_free(css);

	return (0);
}
/* genmon_create_control() */

static GObject *genmon_widget_constructor(GType type, guint n_construct_properties,
                                          GObjectConstructParam *construct_properties)
{
	GObjectClass *parent_class = G_OBJECT_CLASS(genmon_widget_parent_class);
	GObject *obj =
	    parent_class->constructor(type, n_construct_properties, construct_properties);
	GenMonWidget *self = GENMON_WIDGET(obj);
	genmon_widget_build(self);
	genmon_widget_set_timer(self);
	return G_OBJECT(self);
}
static void genmon_widget_init(GenMonWidget *self)
{
	gtk_widget_init_template(GTK_WIDGET(self));
}

static void genmon_widget_set_property(GObject *object, uint prop_id, const GValue *value,
                                       GParamSpec *pspec)
{
	GenMonWidget *self = GENMON_WIDGET(object);
	GtkOrientation orient, invert_orient;
	switch (prop_id)
	{
	case PROP_COMMAND:
		g_clear_pointer(&self->command, g_free);
		self->command = g_value_dup_string(value);
		genmon_widget_display_command_output(self);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_FONT_VALUE:
		g_clear_pointer(&self->font_value, g_free);
		self->font_value = g_value_dup_string(value);
		genmon_widget_set_font_value(self);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_TITLE:
		g_clear_pointer(&self->title, g_free);
		self->title = g_value_dup_string(value);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_UPDATE_INTERVAL_MS:
		if (self->timer_id)
		{
			g_source_remove(self->timer_id);
			self->timer_id = 0;
		}
		self->update_interval_ms = g_value_get_uint(value);
		genmon_widget_set_timer(self);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_IS_TITLE_DISPAYED:
		self->is_title_displayed = g_value_get_boolean(value);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_ORIENTATION:
		orient        = (GtkOrientation)g_value_get_enum(value);
		invert_orient = orient == GTK_ORIENTATION_HORIZONTAL ? GTK_ORIENTATION_VERTICAL
		                                                     : GTK_ORIENTATION_HORIZONTAL;
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->main_box), orient);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->image_box), orient);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->progress), invert_orient);
		gtk_progress_bar_set_inverted(self->progress, invert_orient);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->level), invert_orient);
		gtk_level_bar_set_inverted(self->level, invert_orient);
		g_object_notify_by_pspec(object, pspec);
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static void genmon_widget_get_property(GObject *object, uint prop_id, GValue *value,
                                       GParamSpec *pspec)
{
	GenMonWidget *self = GENMON_WIDGET(object);
	switch (prop_id)
	{
	case PROP_COMMAND:
		g_value_set_string(value, self->command);
		break;
	case PROP_FONT_VALUE:
		g_value_set_string(value, self->font_value);
		break;
	case PROP_TITLE:
		g_value_set_string(value, self->title);
		break;
	case PROP_UPDATE_INTERVAL_MS:
		g_value_set_uint(value, self->update_interval_ms);
		break;
	case PROP_IS_TITLE_DISPAYED:
		g_value_set_boolean(value, self->is_title_displayed);
		break;
	case PROP_ORIENTATION:
		g_value_set_enum(value,
		                 gtk_orientable_get_orientation(GTK_ORIENTABLE(self->main_box)));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

GenMonWidget *genmon_widget_new()
{
	return GENMON_WIDGET(g_object_new(genmon_widget_get_type(), NULL));
}

static void genmon_widget_destroy(GObject *obj)
{
	GenMonWidget *self = GENMON_WIDGET(obj);
    if (self->timer_id != 0)
		g_source_remove(self->timer_id);
	self->timer_id = 0;
}

static void genmon_widget_finalize(GObject *obj)
{
	GenMonWidget *self = GENMON_WIDGET(obj);
	g_clear_pointer(&self->command, g_free);
	g_clear_pointer(&self->title, g_free);
	g_clear_pointer(&self->font_value, g_free);
	g_clear_pointer(&self->cmd_result, g_free);
	g_clear_pointer(&self->click_command, g_free);
	g_clear_pointer(&self->value_click_command, g_free);
}

static void genmon_widget_class_init(GenMonWidgetClass *klass)
{
	GObjectClass *oclass       = G_OBJECT_CLASS(klass);
	oclass->constructor        = genmon_widget_constructor;
	oclass->finalize           = genmon_widget_finalize;
	oclass->set_property       = genmon_widget_set_property;
	oclass->get_property       = genmon_widget_get_property;
	oclass->dispose            = genmon_widget_destroy;

	g_object_class_override_property(oclass, PROP_ORIENTATION, "orientation");
	pspecs[PROP_COMMAND] =
	    g_param_spec_string(GENMON_PROP_CMD,
	                        GENMON_PROP_CMD,
	                        GENMON_PROP_CMD,
	                        "",
	                        (GParamFlags)(G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE |
	                                      G_PARAM_CONSTRUCT));
	pspecs[PROP_FONT_VALUE] =
	    g_param_spec_string(GENMON_PROP_FONT,
	                        GENMON_PROP_FONT,
	                        GENMON_PROP_FONT,
	                        "",
	                        (GParamFlags)(G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE |
	                                      G_PARAM_CONSTRUCT));
	pspecs[PROP_TITLE] =
	    g_param_spec_string(GENMON_PROP_TITLE_TEXT,
	                        GENMON_PROP_TITLE_TEXT,
	                        GENMON_PROP_TITLE_TEXT,
	                        "(x)",
	                        (GParamFlags)(G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE |
	                                      G_PARAM_CONSTRUCT));
	pspecs[PROP_UPDATE_INTERVAL_MS] =
	    g_param_spec_uint(GENMON_PROP_UPDATE_PERIOD,
	                      GENMON_PROP_UPDATE_PERIOD,
	                      GENMON_PROP_UPDATE_PERIOD,
	                      200,
	                      G_MAXUINT,
	                      30 * 1000,
	                      (GParamFlags)(G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE |
	                                    G_PARAM_CONSTRUCT));

	pspecs[PROP_IS_TITLE_DISPAYED] =
	    g_param_spec_boolean(GENMON_PROP_USE_TITLE,
	                         GENMON_PROP_USE_TITLE,
	                         GENMON_PROP_USE_TITLE,
	                         FALSE,
	                         (GParamFlags)(G_PARAM_STATIC_STRINGS | G_PARAM_READWRITE |
	                                       G_PARAM_CONSTRUCT));
	g_object_class_install_properties(oclass, LAST_PROP, pspecs);
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(klass), "/org/genmon/main.ui");
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "main-box",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, main_box));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "title-label",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, title_label));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "image-box",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, image_box));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "progress",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, progress));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "level",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, level));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "button",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, button));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "image",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, image));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "button-image",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, button_image));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "value-label",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, value_label));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "value-button",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget, value_button));
	gtk_widget_class_bind_template_child_full(GTK_WIDGET_CLASS(klass),
	                                          "value-button-label",
	                                          FALSE,
	                                          G_STRUCT_OFFSET(GenMonWidget,
	                                                          value_button_label));
	gtk_widget_class_bind_template_callback_full(GTK_WIDGET_CLASS(klass),
	                                             "genmon_widget_exec_on_click_cmd",
	                                             G_CALLBACK(genmon_widget_exec_on_click_cmd));
	gtk_widget_class_bind_template_callback_full(GTK_WIDGET_CLASS(klass),
	                                             "genmon_widget_exec_on_val_click_cmd",
	                                             G_CALLBACK(
	                                                 genmon_widget_exec_on_val_click_cmd));
}
