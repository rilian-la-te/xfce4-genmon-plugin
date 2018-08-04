#ifndef VALAPANELGENMON_H
#define VALAPANELGENMON_H

#include <vala-panel/client.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(GenMonApplet, genmon_applet, GENMON, APPLET, ValaPanelApplet);

G_DECLARE_FINAL_TYPE(GenMonPlugin, genmon_plugin, GENMON, PLUGIN, ValaPanelAppletPlugin);

G_END_DECLS

#endif // VALAPANELGENMON_H
