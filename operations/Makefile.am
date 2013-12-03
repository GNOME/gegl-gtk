libgeglgtk = $(top_builddir)/gegl-gtk/libgegl-gtk$(GEGL_GTK_GTK_VERSION)-$(GEGL_GTK_API_VERSION).la

ops =

LIBS = $(GTK_LIBS) $(GEGL_LIBS) $(libgeglgtk)
AM_CPPFLAGS = -I. -I$(top_builddir) -I$(top_srcdir)/gegl-gtk -I$(srcdir)
AM_CFLAGS = $(GTK_CFLAGS) $(GEGL_CFLAGS)
AM_LDFLAGS = -avoid-version -export-dynamic -module

gegl_gtk@GEGL_GTK_GTK_VERSION@_display_la_SOURCES = gegl-gtk-display.c
ops += gegl-gtk@GEGL_GTK_GTK_VERSION@-display.la

opdir = $(GEGL_PLUGINS_DIR)
op_LTLIBRARIES = $(ops)
