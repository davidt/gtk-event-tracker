moduledir = $(libdir)/gtk-2.0/modules

libgtkeventtracker_la_SOURCES = \
	module.c

libgtkeventtracker_la_LDFLAGS = \
	-rpath $(moduledir) -module -avoid-version \
	$(GTK_LIBS) \
	@LDFLAGS@

module_LTLIBRARIES = libgtkeventtracker.la

INCLUDES = \
	$(GTK_CFLAGS)
