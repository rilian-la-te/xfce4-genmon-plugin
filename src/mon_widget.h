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

#ifndef MON_WIDGET_H
#define MON_WIDGET_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

/* Configuration Keywords */
#define GENMON_USE_TITLE "use-title"
#define GENMON_TITLE_TEXT "title-text"
#define GENMON_CMD "command"
#define GENMON_UPDATE_PERIOD "update-period"
#define GENMON_FONT "font"

G_DECLARE_FINAL_TYPE(GenMonWidget, genmon_widget, GENMON, WIDGET, GtkEventBox)

GenMonWidget *genmon_widget_new();
void genmon_widget_display_command_output(GenMonWidget *self);

G_END_DECLS

#endif // MON_WIDGET_H
