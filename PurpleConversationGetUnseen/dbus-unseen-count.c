/*
 *   This plugin exposes pidgin-specific PurpleConversation's data with key "unseen-count".
 *   After enabling this plugin, the following command should work from the command line:
 *
 *   prompt$ purple-send PurpleConversationGetUnseen int32:7027118
 *
 *     returns: int 2
 *
 * Purple is the legal property of its developers, whose names are too numerous
 * to list here.  Please refer to the COPYRIGHT file distributed with this
 * source distribution.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111-1301  USA
 */

#include "internal.h"

#include "blist.h"
#include "notify.h"
#include "plugin.h"
#include "version.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DBUS_API_SUBJECT_TO_CHANGE
#include "dbus-maybe.h"
#include "dbus-bindings.h"

DBUS_EXPORT int purple_conversation_get_unseen(PurpleConversation *conv);

#include "dbus-unseen_count-bindings.c"

int purple_conversation_get_unseen(PurpleConversation *conv)
{
	return GPOINTER_TO_INT(purple_conversation_get_data(conv, "unseen-count"));
}

/* And now standard plugin stuff */

static gboolean
plugin_load(PurplePlugin *plugin)
{
	PURPLE_DBUS_RETURN_FALSE_IF_DISABLED(plugin);

	/* First, we have to register our four exported functions with the
	   main purple dbus loop.  Without this statement, the purple dbus
	   code wouldn't know about our functions. */
	PURPLE_DBUS_REGISTER_BINDINGS(plugin);

	return TRUE;
}


static gboolean
plugin_unload(PurplePlugin *plugin)
{
	return TRUE;
}

static PurplePluginInfo info =
{
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,                             /**< type           */
	NULL,                                             /**< ui_requirement */
	0,                                                /**< flags          */
	NULL,                                             /**< dependencies   */
	PURPLE_PRIORITY_DEFAULT,                            /**< priority       */

	"dbus-unseen-count",                                   /**< id             */
	N_("DBus PurpleConversationGetUnseen"),                /**< name           */
	DISPLAY_VERSION,                                  /**< version        */
	                                                  /**  summary        */
	N_("DBus Plugin that exposes PurpleConversationGetUnseen"),
	                                                  /**  description    */
	N_("DBus Plugin that exposes PurpleConversationGetUnseen"),
	"themylogin (http://thelogin.ru)",   /**< author         */
	PURPLE_WEBSITE,                                     /**< homepage       */

	plugin_load,                                      /**< load           */
	plugin_unload,                                    /**< unload         */
	NULL,                                             /**< destroy        */

	NULL,                                             /**< ui_info        */
	NULL,                                             /**< extra_info     */
	NULL,                                       /**< prefs_info     */
	NULL,

	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin)
{
}

PURPLE_INIT_PLUGIN(dbus_unseen_count, init_plugin, info)
