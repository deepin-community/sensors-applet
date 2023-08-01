## Process this file with automake to produce Makefile.in

sensors_applet_libdir=${pkglibdir}/${LIBPANEL_APPLET_API_VERSION}
sensors_applet_lib_LTLIBRARIES=libsensors-applet.la

libsensors_applet_la_CPPFLAGS = \
	-DGNOMELOCALEDIR=\""$(datadir)/locale/"\" \
	-DG_LOG_DOMAIN=\""Sensors Applet"\" \
	-DPIXMAPS_DIR=\""$(datadir)/pixmaps/$(PACKAGE)/"\" \
	-DDATADIR=\""$(datadir)/sensors-applet"\" \
	-DLIBDIR=\""$(libdir)"\" \
	-DUIDIR=\""$(pkgdatadir)/ui"\" \
	-DSENSORS_APPLET_PLUGIN_DIR=\""$(libdir)/$(PACKAGE)/plugins/"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DPREFIX=\""$(prefix)"\" \
	-I$(top_srcdir)

libsensors_applet_la_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(GTK_CFLAGS) \
	$(GNOME_CFLAGS) \
	$(CAIRO_CFLAGS) \
	$(LIBNOTIFY_CFLAGS)

if LIBNOTIFY
libnotify_SRC = active-sensor-libnotify.c \
		active-sensor-libnotify.h
else
libnotify_SRC =
endif

libsensors_applet_la_SOURCES = main.c \
		about-dialog.c \
		about-dialog.h \
		active-sensor.c \
		active-sensor.h \
		prefs-dialog.c \
		prefs-dialog.h \
		sensor-config-dialog.c \
		sensor-config-dialog.h \
		sensors-applet.c \
		sensors-applet.h \
		sensors-applet-sensor.h \
		sensors-applet-plugins.c \
		sensors-applet-plugins.h \
		sensors-applet-conf.c \
		sensors-applet-conf.h \
		$(libnotify_SRC)

libsensors_applet_la_LIBADD = \
	$(GLIB_LIBS) \
	$(GTK_LIBS) \
	$(GNOME_LIBS) \
	$(CAIRO_LIBS) \
	$(LIBNOTIFY_LIBS) -ldl

# install headers for plugins to use
INST_H_FILES = sensors-applet-plugin.h sensors-applet-sensor.h

headerdir = $(prefix)/include/sensors-applet

header_DATA = $(INST_H_FILES)

gsettings_schemas_in_in = \
	org.gnome.sensors-applet.gschema.xml.in.in

@INTLTOOL_XML_NOMERGE_RULE@

gsettings_schemas_in = $(gsettings_schemas_in_in:.xml.in.in=.xml.in)
gsettings_SCHEMAS = $(gsettings_schemas_in:.xml.in=.xml)

%.gschema.xml.in: %.gschema.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@GETTEXT_PACKAGE\@^$(GETTEXT_PACKAGE)^g' < $< > $@

@GSETTINGS_RULES@

# since it is separate
EXTRA_DIST = sensors-applet-plugin.h
