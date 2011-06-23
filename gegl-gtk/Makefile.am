
headers = gegl-gtk.h gegl-gtk-view.h

gegl_gtk_includedir=$(includedir)/gegl-gtk$(GEGL_GTK_GTK_VERSION)-$(GEGL_GTK_API_VERSION)
gegl_gtk_include_HEADERS = $(headers)

gegl_gtk_SOURCES = $(headers) gegl-gtk-view.c
gegl_gtk_LIBADD  = $(GTK_LIBS) $(GEGL_LIBS)
gegl_gtk_CFLAGS  = $(GTK_CFLAGS) $(GEGL_CFLAGS)

if HAVE_GTK2
lib_LTLIBRARIES = libgegl-gtk2-0.1.la
libgegl_gtk2_0_1_la_LIBADD = $(gegl_gtk_LIBADD)
libgegl_gtk2_0_1_la_CFLAGS = $(gegl_gtk_CFLAGS)
libgegl_gtk2_0_1_la_LDFLAGS = $(gegl_gtk_LDFLAGS) -version-info $(GEGL_GTK_LIBRARY_VERSION)
libgegl_gtk2_0_1_la_SOURCES = $(gegl_gtk_SOURCES)
else
lib_LTLIBRARIES = libgegl-gtk3-0.1.la
libgegl_gtk3_0_1_la_LIBADD = $(gegl_gtk_LIBADD)
libgegl_gtk3_0_1_la_CFLAGS = $(gegl_gtk_CFLAGS)
libgegl_gtk3_0_1_la_LDFLAGS = $(gegl_gtk_LDFLAGS) -version-info $(GEGL_GTK_LIBRARY_VERSION)
libgegl_gtk3_0_1_la_SOURCES = $(gegl_gtk_SOURCES)
endif
