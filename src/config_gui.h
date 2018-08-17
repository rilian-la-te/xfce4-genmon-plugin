/*
 *  Generic Monitor plugin for the Xfce4 panel
 *  Header file to construct the configure GUI
 *  Copyright (c) 2004 Roger Seguin <roger_seguin@msn.com>
 *                                  <http://rmlx.dyndns.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.

 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _config_gui_h
#define _config_gui_h

#include <gtk/gtk.h>

#include "mon_widget.h"

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(GenMonConfig, genmon_config, GENMON, CONFIG, GtkGrid)

GenMonConfig *genmon_config_new();
void genmon_config_init_gsettings(GenMonConfig *ui, GSettings *settings);
void genmon_config_init_properties(GenMonConfig *ui, GenMonWidget *widget);

G_END_DECLS

#endif /* _config_gui_h */
