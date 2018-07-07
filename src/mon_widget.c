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

typedef struct
{
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
} GenMonXmlResults;

struct _GenMonWidget
{
	GtkEventBox __parent__;
	unsigned int timer_id; /* Cyclic update */
	GenMonConfigurator configuration;
	GenMonXmlResults monitor;
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
	genmon_widget_exec_with_error_dialog(self, self->monitor.click_command);
}

static void genmon_widget_exec_on_val_click_cmd(GtkWidget *unused, void *data)
/* Execute the onClick Command */
{
	GenMonWidget *self = GENMON_WIDGET(data);
	genmon_widget_exec_with_error_dialog(self, self->monitor.value_click_command);
}

/**************************************************************/

static int DisplayCmdOutput(GenMonWidget *self)
/* Launch the command, get its output and display it in the panel-docked
   text field */
{
	GenMonProperties *props = &(self->configuration.props);
	GenMonXmlResults *res   = &(self->monitor);
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
		gtk_image_set_from_file(GTK_IMAGE(res->image), buf);
		gtk_image_set_from_file(GTK_IMAGE(res->button_image), buf);
		g_free(buf);

		/* Test if the result has a clickable Image (button) */
		begin = strstr(self->cmd_result, "<click>");
		end   = strstr(self->cmd_result, "</click>");
		if (begin && end && begin < end)
		{
			/* Get the command path */
			g_free(res->click_command);
			res->click_command = g_strndup(begin + 7, end - begin - 7);

			gtk_widget_show(GTK_WIDGET(res->button));
			gtk_widget_show(GTK_WIDGET(res->button_image));
			gtk_widget_hide(GTK_WIDGET(res->image));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(res->button));
			gtk_widget_hide(GTK_WIDGET(res->button_image));
			gtk_widget_show(GTK_WIDGET(res->image));
		}
		newVersion = true;
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(res->button));
		gtk_widget_hide(GTK_WIDGET(res->button_image));
		gtk_widget_hide(GTK_WIDGET(res->image));
	}

	/* Test if the result is a Text */
	begin = strstr(self->cmd_result, "<txt>");
	end   = strstr(self->cmd_result, "</txt>");
	if (begin && end && begin < end)
	{
		/* Get the text */
		g_autofree char *buf = g_strndup(begin + 5, end - begin - 5);
		gtk_label_set_markup(GTK_LABEL(res->value_label), buf);

		/* Test if the result has a clickable Value (button) */
		begin = strstr(self->cmd_result, "<txtclick>");
		end   = strstr(self->cmd_result, "</txtclick>");
		if (begin && end && begin < end)
		{
			/* Add the text to the button label too*/
			gtk_label_set_markup(GTK_LABEL(res->value_button_label), buf);

			/* Get the command path */
			g_free(res->value_click_command);
			res->value_click_command = g_strndup(begin + 10, end - begin - 10);

			gtk_widget_show(GTK_WIDGET(res->value_button));
			gtk_widget_show(GTK_WIDGET(res->value_button_label));
			gtk_widget_hide(GTK_WIDGET(res->value_label));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(res->value_button));
			gtk_widget_hide(GTK_WIDGET(res->value_button_label));
			gtk_widget_show(GTK_WIDGET(res->value_label));
		}

		newVersion = true;
	}
	else
	{
		gtk_widget_hide(GTK_WIDGET(res->value_button));
		gtk_widget_hide(GTK_WIDGET(res->value_button_label));
		gtk_widget_hide(GTK_WIDGET(res->value_label));
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
		gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(res->progress),
		                              (float)value / 100.0);
		gtk_widget_show(GTK_WIDGET(res->progress));

		newVersion = true;
	}
	else
		gtk_widget_hide(GTK_WIDGET(res->progress));

	if (!newVersion)
	{
		gtk_widget_show(GTK_WIDGET(res->value_label));
		gtk_label_set_text(GTK_LABEL(res->value_label), self->cmd_result);
	}

	/* Test if a ToolTip is given */
	begin                       = strstr(self->cmd_result, "<tool>");
	end                         = strstr(self->cmd_result, "</tool>");
	g_autofree char *acToolTips = (begin && end && begin < end)
	                                  ? g_strndup(begin + 6, end - begin - 6)
	                                  : acToolTips = g_strdup_printf(
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
static bool SetTimer(void *data)
{
	GenMonWidget *self      = GENMON_WIDGET(data);
	GenMonProperties *props = &(self->configuration.props);

	DisplayCmdOutput(self);

	if (self->timer_id == 0)
	{
		self->timer_id =
		    g_timeout_add(props->update_interval_ms, (GSourceFunc)SetTimer, self);
		return false;
	}

	return true;
} /* SetTimer() */

static void genmon_widget_init(GenMonWidget *self)
{
}

static void genmon_widget_set_property(GObject *object, guint prop_id, const GValue *value,
                                       GParamSpec *pspec)
{
}

static void genmon_widget_get_property(GObject *object, guint prop_id, GValue *value,
                                       GParamSpec *pspec)
{
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
	g_clear_pointer(&self->monitor.click_command, g_free);
	g_clear_pointer(&self->monitor.value_click_command, g_free);
}

static void genmon_widget_class_init(GenMonWidgetClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);
	object_class->finalize     = genmon_widget_finalize;
	object_class->set_property = genmon_widget_set_property;
	object_class->get_property = genmon_widget_get_property;

	g_object_class_override_property(object_class, PROP_ORIENTATION, "orientation");
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
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROP_COMMAND, pspecs[PROP_COMMAND]);
	g_object_class_install_property(G_OBJECT_CLASS(klass),
	                                PROP_FONT_VALUE,
	                                pspecs[PROP_FONT_VALUE]);
	g_object_class_install_property(G_OBJECT_CLASS(klass),
	                                PROP_IS_TITLE_DISPAYED,
	                                pspecs[PROP_IS_TITLE_DISPAYED]);
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROP_TITLE, pspecs[PROP_TITLE]);
	g_object_class_install_property(G_OBJECT_CLASS(klass),
	                                PROP_UPDATE_INTERVAL_MS,
	                                pspecs[PROP_UPDATE_INTERVAL_MS]);
}
