ACLOCAL_AMFLAGS=-I m4

SUBDIRS = sensors-applet lib plugins pixmaps po help

appletdir = $(LIBPANEL_APPLET_DIR)
applet_in_files = org.gnome.applets.SensorsApplet.panel-applet.in
applet_DATA = $(applet_in_files:.panel-applet.in=.panel-applet)

APPLET_LOCATION=$(pkglibdir)/$(LIBPANEL_APPLET_API_VERSION)/libsensors-applet.so

$(applet_in_files): $(applet_in_files).in Makefile
	$(AM_V_GEN)sed \
	-e "s|\@APPLET_LOCATION\@|$(APPLET_LOCATION)|" \
	-e "s|\@VERSION\@|$(PACKAGE_VERSION)|" \
	$< > $@

@INTLTOOL_SERVER_RULE@

uidir 		= $(pkgdatadir)/ui
ui_DATA 	= SensorsApplet.xml

%.panel-applet: %.panel-applet.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

CLEANFILES = $(applet_DATA) $(applet_DATA).in $(schemas_DATA)

EXTRA_DIST 	= $(ui_DATA) \
		  $(applet_in_files:.panel-applet.in=.panel-applet.in.in) \
		  $(service_in_files) \
		  intltool-merge.in intltool-extract.in intltool-update.in

DISTCLEANFILES	=	intltool-extract \
		   	intltool-merge \
	   	   	intltool-update

DISTCHECK_CONFIGURE_FLAGS	= --disable-scrollkeeper
