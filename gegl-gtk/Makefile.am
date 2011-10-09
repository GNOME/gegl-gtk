CLEANFILES = 

gen_sources = gegl-gtk-enums.c
CLEANFILES += $(gen_sources)

headers = gegl-gtk.h gegl-gtk-view.h gegl-gtk-enums.h
sources = gegl-gtk-view.c $(gen_sources)
INCLUDES = $(GTK_CFLAGS) $(GEGL_CFLAGS)

internal_headers = internal/view-helper.h
internal_sources = internal/view-helper.c

gegl-gtk-enums.c: $(srcdir)/gegl-gtk-enums.h
	glib-mkenums \
			   --fhead "/* This is a generated file, do not edit directly */\n\n#include \"config.h\"\n#include <glib-object.h>\n#include \"gegl-gtk-enums.h\"" \
			   --fprod "\n/* enumerations from \"@filename@\" */" \
			   --vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n	static const G@Type@Value values[] = {" \
			   --vprod "	  { @VALUENAME@, \"@valuenick@\", \"@valuenick@\" }," \
			   --vtail "	  { 0, NULL, NULL }\n	};\n	etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n\n" \
			   $(srcdir)/gegl-gtk-enums.h > gegl-gtk-enums.c


gegl_gtk_includedir=$(includedir)/gegl-gtk$(GEGL_GTK_GTK_VERSION)-$(GEGL_GTK_API_VERSION)
gegl_gtk_include_HEADERS = $(headers)

gegl_gtk_SOURCES = $(headers) $(sources) $(internal_headers) $(internal_sources)
gegl_gtk_LIBADD  = $(GTK_LIBS) $(GEGL_LIBS)
gegl_gtk_CFLAGS  = $(INCLUDES) $(CFLAGS)

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

### GObject introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all --add-include-path="$(srcdir)" --add-include-path="$(shell $(PKG_CONFIG) --define-variable=datadir="$(datadir)" --variable=girdir gobject-introspection-1.0)" --identifier-prefix=GeglGtk --symbol-prefix=gegl_gtk
INTROSPECTION_COMPILER_ARGS = --includedir="$(srcdir)" --includedir="$(shell $(PKG_CONFIG) --define-variable=datadir="$(datadir)" --variable=girdir gobject-introspection-1.0)"

if HAVE_INTROSPECTION

GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir: libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la Makefile

GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_CFLAGS = $(INCLUDES)
GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_FILES = $(sources) $(headers)
GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_LIBS = libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la

if HAVE_GTK2
GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_INCLUDES = Gtk-2.0 Gegl-0.1 Babl-0.1
else
GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_INCLUDES = Gtk-3.0 Gegl-0.1 Babl-0.1
endif

INTROSPECTION_GIRS += GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir


girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

if HAVE_VALA
gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.vapi: \
	GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir

	$(VAPIGEN) --pkg gtk+-@GEGL_GTK_GTK_VERSION@.0 \
	--library=gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@ \
	GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir

vapidir=$(datadir)/vala/vapi
vapi_DATA = \
	gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.vapi
	gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.deps

endif # HAVE_VALA

endif # HAVE_INTROSPECTION
