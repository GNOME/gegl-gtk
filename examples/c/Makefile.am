libgeglgtk = $(top_builddir)/gegl-gtk/libgegl-gtk$(GEGL_GTK_GTK_VERSION)-$(GEGL_GTK_API_VERSION).la

LIBS = $(GTK_LIBS) $(GEGL_LIBS) $(libgeglgtk)
AM_CPPFLAGS = -I$(top_srcdir)/gegl-gtk
AM_CFLAGS = $(GTK_CFLAGS) $(GEGL_CFLAGS)

bin_PROGRAMS = \
	gegl-gtk-basic \
	gegl-gtk-display-op \
	gegl-gtk-paint \
	gegl-gtk-scroll \
	gegl-gtk-transform \
	gegl-gtk-warp
