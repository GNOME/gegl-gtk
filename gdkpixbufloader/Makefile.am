pixbufmoduledir = $(PIXBUFMODULEDIR)
pixbufmodule_LTLIBRARIES = io-gegl.la

io_gegl_la_SOURCES = io-gegl.c
io_gegl_la_LDFLAGS = -export_dynamic -avoid-version -module -no-undefined
io_gegl_la_LIBADD = $(GDKPIXBUF_LIBS) $(GEGL_LIBS)
io_gegl_la_CPPFLAGS = $(GDKPIXBUF_CFLAGS) $(GEGL_CFLAGS)


