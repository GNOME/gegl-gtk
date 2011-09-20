
if HAVE_VALA

VALA_FILES = $(wildcard $(srcdir)/*.vala)
EXTRA_DIST = $(VALA_FILES)
bins   = $(subst $(srcdir)/,,$(VALA_FILES:.vala=))

vapi_dir = ../../gegl-gtk
vala_pkg = gegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@

all-local: $(bins)

gegl-gtk-basic: gegl-gtk-basic.vala $(vapi_dir)/$(vala_pkg).vapi
	valac --pkg $(vala_pkg) --vapidir=@VAPIDIR@ --vapidir=$(vapi_dir) -o $@ $< 

clean-local:
	rm -f $(bins)

endif # HAVE_VALA
