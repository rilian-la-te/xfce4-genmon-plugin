/*
 * vala-panel
 * Copyright (C) 2015-2016 Konstantin Pugin <ria.freelander@gmail.com>
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

#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "launcher.h"

typedef struct
{
	pid_t pid;
} SpawnData;

static void child_spawn_func(void *data)
{
	setpgid(0, getpgid(getppid()));
}

bool vala_panel_launch(GDesktopAppInfo *app_info, GList *uris, GtkWidget *parent)
{
	g_autoptr(GError) err            = NULL;
	g_autoptr(GAppLaunchContext) cxt = G_APP_LAUNCH_CONTEXT(
	    gdk_display_get_app_launch_context(gtk_widget_get_display(parent)));
	bool ret = g_desktop_app_info_launch_uris_as_manager(G_DESKTOP_APP_INFO(app_info),
	                                                     uris,
	                                                     cxt,
	                                                     G_SPAWN_SEARCH_PATH,
	                                                     child_spawn_func,
	                                                     NULL,
	                                                     NULL,
	                                                     NULL,
	                                                     &err);
	if (err)
		g_warning("%s\n", err->message);
	return ret;
}

bool vala_panel_launch_command_on_screen(const char *command, GtkWidget *parent)
{
	g_autoptr(GError) err            = NULL;
	g_autoptr(GAppLaunchContext) cxt = G_APP_LAUNCH_CONTEXT(
	    gdk_display_get_app_launch_context(gtk_widget_get_display(parent)));
	g_autoptr(GDesktopAppInfo) info = G_DESKTOP_APP_INFO(
	    g_app_info_create_from_commandline(command, NULL, G_APP_INFO_CREATE_NONE, &err));
	if (err)
	{
		g_warning("%s\n", err->message);
		return false;
	}
	return vala_panel_launch(info, NULL, parent);
}
