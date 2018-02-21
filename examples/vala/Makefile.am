
if HAVE_VALA

bin_PROGRAMS = gegl-gtk-basic

gegl_gtk_basic_CFLAGS = \
	$(GEGL_CFLAGS) \
	$(GTK_CFLAGS) \
	-I$(top_builddir)/gegl-gtk \
	-I$(top_srcdir)/gegl-gtk

gegl_gtk_basic_LDFLAGS = \
	$(GEGL_LIBS) \
	$(GTK_LIBS) \
	$(top_builddir)/gegl-gtk/libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la

gegl_gtk_basic_SOURCES = \
	gegl-gtk-basic.vala

gegl_gtk_basic_VALAFLAGS = \
	--vapidir=$(top_builddir)/gegl-gtk \
	--vapidir=$(top_srcdir)/gegl-gtk \
	--pkg gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@

endif # HAVE_VALA
