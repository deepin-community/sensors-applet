/*
 * Copyright (C) 2005-2009 Alex Murray <murray.alex@gmail.com>
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "about-dialog.h"

void about_dialog_open(SensorsApplet *sensors_applet) {
	gchar *translator;
	const gchar *authors[] = {
		"Alex Murray <murray.alex@gmail.com>",
		NULL
	};

	if (strcmp(_("Translator"), "Translator") == 0) {
		translator = NULL;
	} else {
		translator = g_strdup(_("To translator: Put your name here to show up in the About dialog as the translator"));
	}

	/* Construct the about dialog */
	gtk_show_about_dialog(NULL,
			      "icon-name", "sensors-applet",
			      "program-name", PACKAGE_NAME,
			      "version", PACKAGE_VERSION,
			      "copyright", "(C) 2005-2009, Alex Murray <murray.alex@gmail.com>",
			      "authors", authors,
			      "documenters", authors,
			      "translator-credits", translator,
			      "logo-icon-name", SENSORS_APPLET_ICON,
			      "website", "http://sensors-applet.sourceforge.net/",
			      NULL);


	if (translator != NULL) {
		g_free(translator);
	}

}
