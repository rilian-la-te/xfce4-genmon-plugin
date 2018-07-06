#include "mon_widget.h"

#include <stdbool.h>
#include <stdint.h>

#include "config_gui.h"

typedef struct {
  /* Configurable parameters */
  char *command; /* Commandline to spawn */
  bool is_title_displayed;
  char *title;
  uint32_t update_interval_ms;
  char *font_value;
} GenMonProperties;

typedef struct {
  struct gui_t gui_widgets; /* Configuration/option dialog */
  GenMonProperties props;
} GenMonConfigurator;

typedef struct {
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

struct _GenMonWidget {
  GtkEventBox __parent__;
  unsigned int timer_id; /* Cyclic update */
  GenMonConfigurator configuration;
  GenMonXmlResults monitor;
  char *cmd_result; /* Commandline resulting string */
};

static void gtk_orientable_interface_init(GtkOrientableIface *iface);

G_DEFINE_TYPE_WITH_CODE(GenMonWidget, genmon_widget, GTK_TYPE_EVENT_BOX,
                        G_IMPLEMENT_INTERFACE(GTK_TYPE_ORIENTABLE,
                                              gtk_orientable_interface_init))

static void gtk_orientable_interface_init(GtkOrientableIface *iface) {}

static void genmon_widget_init(GenMonWidget *self) {}

static void genmon_widget_class_init(GenMonWidgetClass *klass) {}
