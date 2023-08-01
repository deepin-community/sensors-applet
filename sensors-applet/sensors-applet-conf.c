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

#include "sensors-applet.h"
#include "sensors-applet-conf.h"

#define DEFAULT_TIMEOUT 2000
#define DEFAULT_GRAPH_SIZE 42

static const gchar * const compatible_versions[] = {
        PACKAGE_VERSION, /* always list current version */
	"3.0.0",
        "2.2.7",
        "2.2.6",
        "2.2.5",
	"2.2.4",
        "2.2.3",
	"2.2.2",
};

#define NUM_COMPATIBLE_VERSIONS G_N_ELEMENTS(compatible_versions)

typedef enum {
        SENSORS_APPLET_GCONF_ERROR = 0,
        SENSORS_APPLET_VERSION_ERROR,
} SensorsAppletGConfError;

static const gchar * const error_titles[] = {
        N_("An error occurred loading the stored sensors data"),
        N_("Incompatible sensors configuration found")
};

static const gchar * const error_messages[] = {
        N_("An error has occurred when loading the stored sensors data. "
           "The default values will be used to recover from this error."),

        N_("Unfortunately the previous configuration for GNOME Sensors Applet "
           "is not compatible with this version. The existing sensors data "
           "will be overwritten with the default values for this new version.")
};


static void sensors_applet_conf_set_defaults(SensorsApplet *sensors_applet) {
	g_settings_set_int(sensors_applet->settings, DISPLAY_MODE, DISPLAY_ICON_WITH_VALUE);
	g_settings_set_int(sensors_applet->settings, LAYOUT_MODE, VALUE_BESIDE_LABEL);
	g_settings_set_int(sensors_applet->settings, TEMPERATURE_SCALE, CELSIUS);
	g_settings_set_int(sensors_applet->settings, TIMEOUT, DEFAULT_TIMEOUT);
	g_settings_set_double(sensors_applet->settings, GRAPH_SIZE, DEFAULT_GRAPH_SIZE);
#ifdef HAVE_LIBNOTIFY
	g_settings_set_boolean(sensors_applet->settings, DISPLAY_NOTIFICATIONS, TRUE);
#endif
	g_settings_set_boolean(sensors_applet->settings, IS_SETUP, FALSE);

}

/**
 * Returns TRUE is old_version is one of the compatible versions
 */
static gboolean sensors_applet_conf_is_compatible(const gchar *old_version) {
	guint i;
	for (i = 0; i < NUM_COMPATIBLE_VERSIONS; i++) {
		if (g_ascii_strcasecmp(old_version, compatible_versions[i]) == 0) {
			return TRUE;
		}
	}
	return FALSE;
}


void sensors_applet_conf_setup(SensorsApplet *sensors_applet) {
	gboolean setup = FALSE;
	gchar *old_version;
	GError *error = NULL;

	setup = g_settings_get_boolean(sensors_applet->settings, IS_SETUP);

	if (setup) {
		/* see if setup version matches */
		old_version = g_settings_get_string(sensors_applet->settings,
				SENSORS_APPLET_VERSION);
		/* if versions don't match or there is no saved
                 * version string then need to overwrite old config */

		if (old_version) {
			if (sensors_applet_conf_is_compatible(old_version)) {
				/* previously setup and versions match so use
                                 * old values */
				g_debug("Config data is compatible. Trying to set up sensors from config data");

				if (sensors_applet_conf_setup_sensors(sensors_applet)) {
					g_debug("done setting up from config backend");
				} else {
					g_debug("Setting conf defaults only");
					sensors_applet_conf_set_defaults(sensors_applet);
				}
				g_free(old_version);

				return;


			}
			g_free(old_version);
		}
		sensors_applet_notify(sensors_applet, GCONF_READ_ERROR);


	}

	/* use defaults */
	g_debug("Setting config defaults only");
	sensors_applet_conf_set_defaults(sensors_applet);
}


enum {
        PATHS_INDEX = 0,
        IDS_INDEX,
        LABELS_INDEX,
        INTERFACES_INDEX,
        SENSOR_TYPES_INDEX,
        ENABLES_INDEX,
        LOW_VALUES_INDEX,
        HIGH_VALUES_INDEX,
        ALARM_ENABLES_INDEX,
        LOW_ALARM_COMMANDS_INDEX,
        HIGH_ALARM_COMMANDS_INDEX,
        ALARM_TIMEOUTS_INDEX,
        MULTIPLIERS_INDEX,
        OFFSETS_INDEX,
        ICON_TYPES_INDEX,
        GRAPH_COLORS_INDEX,
        NUM_KEYS
};

const gchar * const keys[NUM_KEYS] = {
	PATHS,
	IDS,
	LABELS,
	INTERFACES,
	SENSOR_TYPES,
	ENABLES,
	LOW_VALUES,
	HIGH_VALUES,
	ALARM_ENABLES,
	LOW_ALARM_COMMANDS,
	HIGH_ALARM_COMMANDS,
	ALARM_TIMEOUTS,
	MULTIPLIERS,
	OFFSETS,
	ICON_TYPES,
	GRAPH_COLORS,
};

/* gets called if are already setup so we don't have to manually go
   through and find sensors etc again */
gboolean sensors_applet_conf_setup_sensors(SensorsApplet *sensors_applet) {
	/* everything gets stored except alarm timeout indexes, which
	   we set to -1, and visible which we set to false for all
	   parent nodes and true for all child nodes */

	GVariantIter *iter;

	gchar *current_path, *current_id, *current_label, *current_interface,
		*current_low_alarm_command, *current_high_alarm_command,
		*current_graph_color;
	gboolean current_enable, current_alarm_enable;
	gdouble current_low_value, current_high_value, current_multiplier,
		current_offset;
	guint32 current_sensor_type, current_alarm_timeout,
		current_icon_type;

	g_settings_get (sensors_applet->settings, "slist", "a(ssssbddbssuuddus)", &iter);

	while (g_variant_iter_loop (iter, "(ssssbddbssuuddus)", &current_path, &current_id, &current_label,
		&current_interface, &current_enable, &current_low_value, &current_high_value, &current_alarm_enable,
		&current_low_alarm_command, &current_high_alarm_command, &current_alarm_timeout, &current_sensor_type,
		&current_multiplier, &current_offset, &current_icon_type, &current_graph_color)) {

		sensors_applet_add_sensor(sensors_applet,
			current_path, current_id, current_label, current_interface,
			current_sensor_type,
			current_enable,
			current_low_value / 1000.0,
			current_high_value / 1000.0,
			current_alarm_enable,
			current_low_alarm_command,
			current_high_alarm_command,
			current_alarm_timeout,
			current_multiplier / 1000.0,
			current_offset / 1000.0,
			current_icon_type, current_graph_color);

	}

	return TRUE;
}


gboolean sensors_applet_conf_save_sensors(SensorsApplet *sensors_applet) {
	/* write everything to conf backend except VISIBLE and
	   ALARM_TIMEOUT_INDEX */
	/* for stepping through GtkTreeStore data structure */
	GtkTreeIter interfaces_iter, sensors_iter;
	gboolean not_end_of_interfaces = TRUE, not_end_of_sensors = TRUE;

	gchar *current_path, *current_id, *current_label, *current_interface,
		*current_low_alarm_command, *current_high_alarm_command,
		*current_graph_color;
	gboolean current_enable, current_alarm_enable;
	gdouble current_low_value, current_high_value, current_multiplier,
		current_offset;
	guint32 current_sensor_type, current_icon_type, current_alarm_timeout;
	GVariant *vval;

	GVariantBuilder builder;

	g_variant_builder_init (&builder, G_VARIANT_TYPE ("a(ssssbddbssuuddus)"));

	/* now step through the GtkTreeStore sensors to
	   find which sensors are enabled */
	for (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(sensors_applet->sensors), &interfaces_iter);
			not_end_of_interfaces;
			not_end_of_interfaces = gtk_tree_model_iter_next(GTK_TREE_MODEL(sensors_applet->sensors), &interfaces_iter)) {
		// store a settings key for this interface
		gtk_tree_model_get(GTK_TREE_MODEL(sensors_applet->sensors),
			&interfaces_iter,
			ID_COLUMN, &current_id,
			-1);

		g_settings_set_boolean(sensors_applet->settings, current_id, TRUE);
		g_free(current_id);

		/* reset sensors sentinel */
		not_end_of_sensors = TRUE;

		for (gtk_tree_model_iter_children(GTK_TREE_MODEL(sensors_applet->sensors), &sensors_iter, &interfaces_iter);
				not_end_of_sensors;
				not_end_of_sensors = gtk_tree_model_iter_next(GTK_TREE_MODEL(sensors_applet->sensors), &sensors_iter)) {
			gtk_tree_model_get(GTK_TREE_MODEL(sensors_applet->sensors),
				&sensors_iter,
				PATH_COLUMN, &current_path,
				ID_COLUMN, &current_id,
				LABEL_COLUMN, &current_label,
				INTERFACE_COLUMN, &current_interface,
				SENSOR_TYPE_COLUMN, &current_sensor_type,
				ENABLE_COLUMN, &current_enable,
				LOW_VALUE_COLUMN, &current_low_value,
				HIGH_VALUE_COLUMN, &current_high_value,
				ALARM_ENABLE_COLUMN, &current_alarm_enable,
				LOW_ALARM_COMMAND_COLUMN, &current_low_alarm_command,
				HIGH_ALARM_COMMAND_COLUMN, &current_high_alarm_command,
				ALARM_TIMEOUT_COLUMN, &current_alarm_timeout,
				MULTIPLIER_COLUMN, &current_multiplier,
				OFFSET_COLUMN, &current_offset,
				ICON_TYPE_COLUMN, &current_icon_type,
				GRAPH_COLOR_COLUMN, &current_graph_color,
				-1);

			vval = g_variant_new("(ssssbddbssuuddus)", g_strdup(current_path),
				g_strdup(current_id), g_strdup(current_label), g_strdup(current_interface),
				current_enable, current_low_value * 1000,
				current_high_value * 1000, current_alarm_enable,
				current_low_alarm_command, current_high_alarm_command,
				current_alarm_timeout, current_sensor_type,
				current_multiplier * 1000, current_offset * 1000,
				current_icon_type, g_strdup(current_graph_color));
			g_variant_builder_add_value(&builder, vval);
		}
	}

	g_settings_set_value (sensors_applet->settings, "slist",
		g_variant_builder_end (&builder));

	/* store current version to identify config data */
	g_settings_set_string(sensors_applet->settings,
		SENSORS_APPLET_VERSION,
		PACKAGE_VERSION);

	return TRUE;
}
