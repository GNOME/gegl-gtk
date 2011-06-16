
headers = gegl-gtk.h gegl-gtk-view.h

gegl_gtk_includedir=$(includedir)/gegl-gtk-$(GEGL_GTK_API_VERSION)
gegl_gtk_include_HEADERS = $(headers)

gegl_gtk_SOURCES = $(headers) gegl-gtk-view.c
gegl_gtk_LIBADD  = $(GTK_LIBS) $(GEGL_LIBS)
gegl_gtk_CFLAGS  = $(GTK_CFLAGS) $(GEGL_CFLAGS)

if HAVE_GTK2
lib_LTLIBRARIES = libgegl-gtk-2.0.la
libgegl_gtk_2_0_la_LIBADD = $(gegl_gtk_LIBADD)
libgegl_gtk_2_0_la_CFLAGS = $(gegl_gtk_CFLAGS)
libgegl_gtk_2_0_la_LDFLAGS = $(gegl_gtk_LDFLAGS) -version-info 0:2:0
libgegl_gtk_2_0_la_SOURCES = $(gegl_gtk_SOURCES)
else
lib_LTLIBRARIES = libgegl-gtk-3.0.la
libgegl_gtk_3_0_la_LIBADD = $(gegl_gtk_LIBADD)
libgegl_gtk_3_0_la_CFLAGS = $(gegl_gtk_CFLAGS)
libgegl_gtk_3_0_la_LDFLAGS = $(gegl_gtk_LDFLAGS) -version-info 0:3:0
libgegl_gtk_3_0_la_SOURCES = $(gegl_gtk_SOURCES)
endif
