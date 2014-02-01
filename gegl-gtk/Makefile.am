
### Library
CLEANFILES = 
EXTRA_DIST =

gen_sources = gegl-gtk-enums.c gegl-gtk-marshal.c
gen_headers = gegl-gtk-marshal.h
CLEANFILES += $(gen_sources) $(gen_headers)
BUILT_SOURCES = $(gen_headers)

headers = gegl-gtk.h gegl-gtk-view.h gegl-gtk-enums.h
sources = gegl-gtk-view.c $(gen_sources)
AM_CFLAGS = $(GTK_CFLAGS) $(GEGL_CFLAGS)

internal_headers = internal/view-helper.h
internal_sources = internal/view-helper.c

gegl_gtk_includedir=$(includedir)/gegl-gtk$(GEGL_GTK_GTK_VERSION)-$(GEGL_GTK_API_VERSION)
gegl_gtk_include_HEADERS = $(headers)

lib_LTLIBRARIES = libgegl-gtk@GEGL_GTK_GTK_VERSION@-0.1.la

libgegl_gtk@GEGL_GTK_GTK_VERSION@_0_1_la_SOURCES = $(headers)  $(gen_headers) $(sources) $(internal_headers) $(internal_sources)
libgegl_gtk@GEGL_GTK_GTK_VERSION@_0_1_la_LIBADD = $(GTK_LIBS) $(GEGL_LIBS) $(CAIRO_GOBJECT_LIBS)
libgegl_gtk@GEGL_GTK_GTK_VERSION@_0_1_la_CFLAGS = $(AM_CFLAGS) $(CFLAGS)
libgegl_gtk@GEGL_GTK_GTK_VERSION@_0_1_la_LDFLAGS = $(gegl_gtk_LDFLAGS) -version-info $(GEGL_GTK_LIBRARY_VERSION)

gegl-gtk-enums.c: $(srcdir)/gegl-gtk-enums.h
	glib-mkenums \
			   --fhead "/* This is a generated file, do not edit directly */\n\n#include \"config.h\"\n#include <glib-object.h>\n#include \"gegl-gtk-enums.h\"" \
			   --fprod "\n/* enumerations from \"@filename@\" */" \
			   --vhead "GType\n@enum_name@_get_type (void)\n{\n  static GType etype = 0;\n  if (etype == 0) {\n	static const G@Type@Value values[] = {" \
			   --vprod "	  { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
			   --vtail "	  { 0, NULL, NULL }\n	};\n	etype = g_@type@_register_static (\"@EnumName@\", values);\n  }\n  return etype;\n}\n\n" \
			   $(srcdir)/gegl-gtk-enums.h > gegl-gtk-enums.c

gegl-gtk-marshal.h: $(srcdir)/gegl-gtk-marshal.list
	glib-genmarshal --prefix=gegl_gtk_marshal $(srcdir)/gegl-gtk-marshal.list --header >> gegl-gtk-marshal.h

gegl-gtk-marshal.c: gegl-gtk-marshal.h
	echo "#include \"gegl-gtk-marshal.h\"" >> gegl-gtk-marshal.c \
	&& glib-genmarshal --prefix=gegl_gtk_marshal $(srcdir)/gegl-gtk-marshal.list --body >> gegl-gtk-marshal.c

EXTRA_DIST += gegl-gtk-marshal.list

### pkg-config
pkgconfigdir = $(libdir)/pkgconfig

if HAVE_GTK2
pkgconfig_DATA = gegl-gtk2-0.1.pc
else
pkgconfig_DATA = gegl-gtk3-0.1.pc
endif

EXTRA_DIST += \
	gegl-gtk2.pc.in \
	gegl-gtk3.pc.in

### GObject introspection
-include $(INTROSPECTION_MAKEFILE)

INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --warn-all --add-include-path="$(srcdir)" --add-include-path="$(shell $(PKG_CONFIG) --define-variable=datadir="$(datadir)" --variable=girdir gobject-introspection-1.0)" --identifier-prefix=GeglGtk --symbol-prefix=gegl_gtk
INTROSPECTION_COMPILER_ARGS = --includedir="$(srcdir)" --includedir="$(shell $(PKG_CONFIG) --define-variable=datadir="$(datadir)" --variable=girdir gobject-introspection-1.0)"

if HAVE_INTROSPECTION

GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir: libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la Makefile

GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_CFLAGS = $(AM_CFLAGS) $(AM_CPPFLAGS)
GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_FILES = $(sources) $(headers)
GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_LIBS = libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la
GeglGtk@GEGL_GTK_GTK_VERSION@_0_1_gir_INCLUDES = Gtk-@GEGL_GTK_GTK_VERSION@.0 Gegl-0.3

INTROSPECTION_GIRS += GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir


girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)

if HAVE_VALA
gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.vapi: \
	GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir

	$(VAPIGEN) --pkg gtk+-@GEGL_GTK_GTK_VERSION@.0 --pkg Gegl-0.3 \
	--library=gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@ \
	--vapidir=@VAPIDIR@ --girdir=@GIRDIR@ \
	GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.gir

vapidir=@VAPIDIR@
vapi_DATA = \
	gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.vapi \
	gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.deps

CLEANFILES += gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.vapi

endif # HAVE_VALA

endif # HAVE_INTROSPECTION

EXTRA_DIST += \
	gegl-gtk2-@GEGL_GTK_API_VERSION@.deps \
	gegl-gtk3-@GEGL_GTK_API_VERSION@.deps

CLEANFILES += \
	GeglGtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.metadata
