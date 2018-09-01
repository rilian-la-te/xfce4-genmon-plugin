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

#ifndef UTIL_H
#define UTIL_H

#include <gio/gdesktopappinfo.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

void vala_panel_setup_button(GtkButton *b, GtkImage *img, const char *label);
void css_apply_with_class(GtkWidget *widget, const char *css, const char *klass, bool remove);
bool vala_panel_launch_with_context(GDesktopAppInfo *app_info, GAppLaunchContext *cxt, GList *uris);

G_END_DECLS

#endif // UTIL_H
