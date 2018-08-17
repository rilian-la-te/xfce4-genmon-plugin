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

#ifndef VALAPANELGENMON_H
#define VALAPANELGENMON_H

#include <gtk/gtk.h>
#include <libxfce4panel/libxfce4panel.h>
#include <stdbool.h>

G_BEGIN_DECLS

GType genmon_applet_get_type(void);
G_GNUC_BEGIN_IGNORE_DEPRECATIONS
typedef struct _GenMonApplet GenMonApplet;
typedef struct
{
	XfcePanelPluginClass parent_class;
} GenMonAppletClass;

static inline GenMonApplet *GENMON_APPLET(gpointer ptr)
{
	return G_TYPE_CHECK_INSTANCE_CAST(ptr, genmon_applet_get_type(), GenMonApplet);
}
static inline bool GENMON_IS_APPLET(gpointer ptr)
{
	return G_TYPE_CHECK_INSTANCE_TYPE(ptr, genmon_applet_get_type());
}
G_GNUC_END_IGNORE_DEPRECATIONS

G_END_DECLS

#endif // VALAPANELGENMON_H
