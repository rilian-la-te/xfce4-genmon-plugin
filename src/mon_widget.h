#ifndef MON_WIDGET_H
#define MON_WIDGET_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Configuration Keywords */
#define GENMON_PROP_USE_LABEL "use-label"
#define GENMON_PROP_LABEL_TEXT "label-text"
#define GENMON_PROP_CMD "command"
#define GENMON_PROP_UPDATE_PERIOD "update-period"
#define GENMON_PROP_FONT "font"

G_DECLARE_FINAL_TYPE(GenMonWidget, genmon_widget, GENMON, WIDGET, GtkEventBox)

G_END_DECLS

#endif // MON_WIDGET_H
