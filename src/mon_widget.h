#ifndef MON_WIDGET_H
#define MON_WIDGET_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Configuration Keywords */
#define GENMON_PROP_USE_TITLE "use-title"
#define GENMON_PROP_TITLE_TEXT "title-text"
#define GENMON_PROP_CMD "command"
#define GENMON_PROP_UPDATE_PERIOD "update-period"
#define GENMON_PROP_FONT "font"

G_DECLARE_FINAL_TYPE(GenMonWidget, genmon_widget, GENMON, WIDGET, GtkEventBox)

GenMonWidget *genmon_widget_new();
void genmon_widget_display_command_output(GenMonWidget *self);

G_END_DECLS

#endif // MON_WIDGET_H
