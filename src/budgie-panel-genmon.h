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

#include <budgie-desktop/applet.h>
#include <budgie-desktop/plugin.h>
#include <libpeas/peas.h>
#include <stdbool.h>

G_BEGIN_DECLS

G_DECLARE_FINAL_TYPE(GenMonPlugin, genmon_plugin, GENMON, PLUGIN, PeasExtensionBase);

G_DECLARE_FINAL_TYPE(GenMonApplet, genmon_applet, GENMON, APPLET, BudgieApplet)

G_END_DECLS

#endif // VALAPANELGENMON_H
