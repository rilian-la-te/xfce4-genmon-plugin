#include "mon_widget.h"

#include <glib/gi18n.h>
#include <stdbool.h>
#include <stdint.h>

#include "cmdspawn.h"
#include "config_gui.h"
#include "launcher.h"

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

typedef struct
{
	/* Configurable parameters */
	char *command; /* Commandline to spawn */
	bool is_title_displayed;
	char *title;
	uint32_t update_interval_ms;
	char *font_value;
} GenMonProperties;

typedef struct
{
	struct gui_t gui_widgets; /* Configuration/option dialog */
	GenMonProperties props;
} GenMonConfigurator;

struct _GenMonWidget
{
	GtkEventBox __parent__;
	unsigned int timer_id; /* Cyclic update */
	GenMonConfigurator configuration;
	/* Plugin monitor */
	GtkBox *main_box;
	GtkBox *image_box;
	GtkLabel *title_label;
	GtkLabel *value_label;
	GtkButton *value_button;
	GtkLabel *value_button_label;
	GtkImage *image;
	GtkProgressBar *progress;
	GtkButton *button;
	GtkImage *button_image;
	char *click_command;
	char *value_click_command;
	char *cmd_result; /* Commandline resulting string */
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
	bool fail =
	    vala_panel_launch_command_on_screen(cmd, gtk_widget_get_parent(GTK_WIDGET(self)));
	if (fail)
	{
		g_autofree char *first = g_strdup_printf(_("Could not run \"%s\""), cmd);
		g_autoptr(GtkDialog) dlg =
		    GTK_DIALOG(gtk_message_dialog_new(NULL,
		                                      GTK_DIALOG_DESTROY_WITH_PARENT,
		                                      GTK_MESSAGE_ERROR,
		                                      GTK_BUTTONS_CLOSE,
		                                      "%s",
		                                      first));
		gtk_dialog_run(dlg);
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

static int genmon_widget_display_command_output(GenMonWidget *self)
/* Launch the command, get its output and display it in the panel-docked
   text field */
{
	GenMonProperties *props = &(self->configuration.props);
	char *begin;
	char *end;
	bool newVersion = false;

	g_clear_pointer(&self->cmd_result, g_free);
	self->cmd_result = props->command[0] > 0 ? genmon_SpawnCmd(props->command, 1) : NULL;

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
			gtk_widget_show(GTK_WIDGET(self->button_image));
			gtk_widget_hide(GTK_WIDGET(self->image));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(self->button));
			gtk_widget_hide(GTK_WIDGET(self->button_image));
			gtk_widget_show(GTK_WIDGET(self->image));
		}
		newVersion = true;
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(self->button));
		gtk_widget_hide(GTK_WIDGET(self->button_image));
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
			gtk_widget_show(GTK_WIDGET(self->value_button_label));
			gtk_widget_hide(GTK_WIDGET(self->value_label));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(self->value_button));
			gtk_widget_hide(GTK_WIDGET(self->value_button_label));
			gtk_widget_show(GTK_WIDGET(self->value_label));
		}

		newVersion = true;
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(self->value_button));
		gtk_widget_hide(GTK_WIDGET(self->value_button_label));
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

	if (!newVersion)
	{
		gtk_widget_show(GTK_WIDGET(self->value_label));
		gtk_label_set_text(GTK_LABEL(self->value_label), self->cmd_result);
	}

	/* Test if a ToolTip is given */
	begin                       = strstr(self->cmd_result, "<tool>");
	end                         = strstr(self->cmd_result, "</tool>");
	g_autofree char *acToolTips = (begin && end && begin < end)
	                                  ? g_strndup(begin + 6, end - begin - 6)
	                                  : g_strdup_printf(
	                                        "%s\n"
	                                        "----------------\n"
	                                        "%s\n"
	                                        "Period (s): %d",
	                                        props->title,
	                                        props->command,
	                                        props->update_interval_ms / 1000);

	gtk_widget_set_tooltip_markup(GTK_WIDGET(self), acToolTips);

	return 0;

} /* DisplayCmdOutput() */

/* Recurrently update the panel-docked monitor through a timer */
/* Warning : should not be called directly (except the 1st time) */
/* To avoid multiple timers */
static bool genmon_widget_set_timer(void *data)
{
	GenMonWidget *self      = GENMON_WIDGET(data);
	GenMonProperties *props = &(self->configuration.props);

	genmon_widget_display_command_output(self);

	if (self->timer_id == 0)
	{
		self->timer_id = g_timeout_add(props->update_interval_ms,
		                               (GSourceFunc)genmon_widget_set_timer,
		                               self);
		return false;
	}

	return true;
} /* SetTimer() */

/* Create the plugin widgets*/
static void genmon_widget_build(GenMonWidget *self)
{
	GtkOrientation orientation = gtk_orientable_get_orientation(GTK_ORIENTABLE(self));

	self->configuration.props.command            = g_strdup("");
	self->configuration.props.title              = g_strdup("(genmon)");
	self->configuration.props.is_title_displayed = false;
	self->configuration.props.update_interval_ms = 30 * 1000;
	self->timer_id                               = 0;
	gtk_event_box_set_visible_window(GTK_EVENT_BOX(self), false);

	gtk_widget_show(GTK_WIDGET(self->main_box));
	gtk_container_set_border_width(GTK_CONTAINER(self->main_box), 0);
	gtk_label_set_text(self->title_label, self->configuration.props.title);
	if (self->configuration.props.is_title_displayed)
		gtk_widget_show(GTK_WIDGET(self->title_label));

	/* Create a Box to put image and text */
	gtk_widget_show(GTK_WIDGET(self->image_box));
	gtk_container_set_border_width(GTK_CONTAINER(self->image_box), 0);

	//	xfce_panel_plugin_add_action_widget(plugin, self->button);

	/* Add Image Button*/
	gtk_container_add(GTK_CONTAINER(self->button), GTK_WIDGET(self->button_image));
	gtk_container_set_border_width(GTK_CONTAINER(self->button), 0);

	/* Add Value */
	gtk_widget_show(GTK_WIDGET(self->value_label));

	/* Add Value Button */
	gtk_container_set_border_width(GTK_CONTAINER(self->value_button), 0);

	/* Add Value Button Label */

	/* Add Bar */
	if (orientation == GTK_ORIENTATION_HORIZONTAL)
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->progress),
		                               GTK_ORIENTATION_VERTICAL);
		gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(self->progress), true);
	}
	else
	{
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->progress),
		                               GTK_ORIENTATION_HORIZONTAL);
		gtk_progress_bar_set_inverted(GTK_PROGRESS_BAR(self->progress), false);
	}

	/* make widget padding consistent */
	g_autofree char *css = g_strdup_printf(
	    "\
            progressbar.horizontal trough { min-height: 6px; }\
            progressbar.horizontal progress { min-height: 6px; }\
            progressbar.vertical trough { min-width: 6px; }\
            progressbar.vertical progress { min-width: 6px; }");

	g_autoptr(GtkCssProvider) css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(css_provider, css, strlen(css), NULL);
	gtk_style_context_add_provider(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(
	                                   GTK_WIDGET(self->progress))),
	                               GTK_STYLE_PROVIDER(css_provider),
	                               GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}
/* genmon_create_control() */

static void genmon_widget_init(GenMonWidget *self)
{
	genmon_widget_build(self);
	genmon_widget_set_timer(self);
}

static void genmon_widget_set_property(GObject *object, uint prop_id, const GValue *value,
                                       GParamSpec *pspec)
{
	GenMonWidget *self =
	    G_TYPE_CHECK_INSTANCE_CAST(object, genmon_widget_get_type(), GenMonWidget);
	GtkOrientation orient;
	switch (prop_id)
	{
	case PROP_COMMAND:
		g_clear_pointer(&self->configuration.props.command, g_free);
		self->configuration.props.command = g_value_get_string(value);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_FONT_VALUE:
		g_clear_pointer(&self->configuration.props.font_value, g_free);
		self->configuration.props.font_value = g_value_get_string(value);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_TITLE:
		g_clear_pointer(&self->configuration.props.title, g_free);
		self->configuration.props.title = g_value_get_string(value);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_UPDATE_INTERVAL_MS:
		if (self->timer_id)
		{
			g_source_remove(self->timer_id);
			self->timer_id = 0;
		}
		self->configuration.props.update_interval_ms = g_value_get_uint(value);
		genmon_widget_set_timer(self);
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_IS_TITLE_DISPAYED:
		self->configuration.props.is_title_displayed = g_value_get_boolean(value);
		if (self->configuration.props.is_title_displayed)
			gtk_widget_show(GTK_WIDGET(self->title_label));
		else
			gtk_widget_hide(GTK_WIDGET(self->title_label));
		g_object_notify_by_pspec(object, pspec);
		break;
	case PROP_ORIENTATION:
		orient = (GtkOrientation)g_value_get_enum(value);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->main_box), orient);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->image_box), orient);
		gtk_orientable_set_orientation(GTK_ORIENTABLE(self->progress),
		                               orient == GTK_ORIENTATION_HORIZONTAL
		                                   ? GTK_ORIENTATION_VERTICAL
		                                   : GTK_ORIENTATION_HORIZONTAL);
		gtk_progress_bar_set_inverted(self->progress,
		                              orient == GTK_ORIENTATION_HORIZONTAL ? true : false);
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
		g_value_set_string(value, self->configuration.props.command);
		break;
	case PROP_FONT_VALUE:
		g_value_set_string(value, self->configuration.props.font_value);
		break;
	case PROP_TITLE:
		g_value_set_string(value, self->configuration.props.title);
		break;
	case PROP_UPDATE_INTERVAL_MS:
		g_value_set_uint(value, self->configuration.props.update_interval_ms);
		break;
	case PROP_IS_TITLE_DISPAYED:
		g_value_set_boolean(value, self->configuration.props.is_title_displayed);
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
static void genmon_widget_finalize(GObject *obj)
{
	GenMonWidget *self = GENMON_WIDGET(obj);
	if (self->timer_id)
		g_source_remove(self->timer_id);

	g_clear_pointer(&self->configuration.props.command, g_free);
	g_clear_pointer(&self->configuration.props.title, g_free);
	g_clear_pointer(&self->configuration.props.font_value, g_free);
	g_clear_pointer(&self->cmd_result, g_free);
	g_clear_pointer(&self->click_command, g_free);
	g_clear_pointer(&self->value_click_command, g_free);
}

static void genmon_widget_class_init(GenMonWidgetClass *klass)
{
	GObjectClass *oclass = G_OBJECT_CLASS(klass);
	oclass->finalize     = genmon_widget_finalize;
	oclass->set_property = genmon_widget_set_property;
	oclass->get_property = genmon_widget_get_property;

	g_object_class_override_property(oclass, PROP_ORIENTATION, "orientation");
	pspecs[PROP_COMMAND] =
	    g_param_spec_string(GENMON_PROP_CMD,
	                        GENMON_PROP_CMD,
	                        GENMON_PROP_CMD,
	                        NULL,
	                        (GParamFlags)(G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
	                                      G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
	                                      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
	pspecs[PROP_FONT_VALUE] =
	    g_param_spec_string(GENMON_PROP_FONT,
	                        GENMON_PROP_FONT,
	                        GENMON_PROP_FONT,
	                        NULL,
	                        (GParamFlags)(G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
	                                      G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
	                                      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
	pspecs[PROP_TITLE] =
	    g_param_spec_string(GENMON_PROP_TITLE_TEXT,
	                        GENMON_PROP_TITLE_TEXT,
	                        GENMON_PROP_TITLE_TEXT,
	                        NULL,
	                        (GParamFlags)(G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
	                                      G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
	                                      G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
	pspecs[PROP_UPDATE_INTERVAL_MS] =
	    g_param_spec_uint(GENMON_PROP_UPDATE_PERIOD,
	                      GENMON_PROP_UPDATE_PERIOD,
	                      GENMON_PROP_UPDATE_PERIOD,
	                      1,
	                      G_MAXUINT,
	                      100,
	                      (GParamFlags)(G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
	                                    G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
	                                    G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));

	pspecs[PROP_IS_TITLE_DISPAYED] =
	    g_param_spec_boolean(GENMON_PROP_USE_TITLE,
	                         GENMON_PROP_USE_TITLE,
	                         GENMON_PROP_USE_TITLE,
	                         FALSE,
	                         (GParamFlags)(G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
	                                       G_PARAM_STATIC_BLURB | G_PARAM_READABLE |
	                                       G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
	g_object_class_install_property(oclass, PROP_COMMAND, pspecs[PROP_COMMAND]);
	g_object_class_install_property(oclass, PROP_FONT_VALUE, pspecs[PROP_FONT_VALUE]);
	g_object_class_install_property(oclass,
	                                PROP_IS_TITLE_DISPAYED,
	                                pspecs[PROP_IS_TITLE_DISPAYED]);
	g_object_class_install_property(oclass, PROP_TITLE, pspecs[PROP_TITLE]);
	g_object_class_install_property(oclass,
	                                PROP_UPDATE_INTERVAL_MS,
	                                pspecs[PROP_UPDATE_INTERVAL_MS]);
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
