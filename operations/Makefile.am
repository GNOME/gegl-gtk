
ext_dir = $(GEGL_PLUGINS_DIR)
libgeglgtk = $(top_builddir)/gegl-gtk/libgegl-gtk$(GEGL_GTK_GTK_VERSION)-$(GEGL_GTK_API_VERSION).la

op_LIBS            = $(GTK_LIBS) $(GEGL_LIBS) $(libgeglgtk)
op_CFLAGS			= $(GTK_CFLAGS) $(GEGL_CFLAGS)
op_LDFLAGS			= -avoid-version -export-dynamic -module

CFILES          = $(wildcard $(srcdir)/*.c)
plugins         = $(subst gegl-gtk-,gegl-gtk@GEGL_GTK_GTK_VERSION@-, $(subst $(srcdir)/,,$(CFILES:.c=.la)))
INSTALLED_ITEMS = $(subst gegl-gtk-,gegl-gtk@GEGL_GTK_GTK_VERSION@-, $(subst $(srcdir)/,$(ext_dir)/,$(CFILES:.c=$(SHREXT))))
EXTRA_DIST      = $(wildcard *.c) $(wildcard *.h)

gegl_lt_cc_v = $(gegl_lt_cc_v_$(V))
gegl_lt_cc_v_ = $(gegl_lt_cc_v_$(AM_DEFAULT_VERBOSITY))
gegl_lt_cc_v_0 = @echo "  CC     " $<;
gegl_lt_ccld_v = $(gegl_lt_ccld_v_$(V))
gegl_lt_ccld_v_ = $(gegl_lt_ccld_v_$(AM_DEFAULT_VERBOSITY))
gegl_lt_ccld_v_0 = @echo "  CCLD   " $@;

all-local: $(plugins)

gegl-gtk@GEGL_GTK_GTK_VERSION@-%.la: gegl-gtk-%.c
	$(gegl_lt_cc_v) $(LIBTOOL) --quiet --tag=CC --mode=compile $(CC) $(DEFS) -I. -I$(top_builddir) -I$(top_srcdir)/gegl-gtk -I$(srcdir) $(AM_CPPFLAGS) $(CPPFLAGS) $(op_CFLAGS) $(AM_CFLAGS) $(CFLAGS) -c -o $@.lo $<
	$(gegl_lt_ccld_v) $(LIBTOOL) --quiet --tag=CC --mode=link $(CC) $(AM_LDFLAGS) $(op_LDFLAGS) $(op_CFLAGS) $(CFLAGS) -o $@ -rpath $(ext_dir) $@.lo $(op_LIBS) $(MATH_LIB)

clean-local:
	rm -f *.la

install-exec-local: all-local
	$(INSTALL) -d $(DESTDIR)$(ext_dir)
	for i in $(plugins); do \
	  $(LIBTOOL) --mode=install $(INSTALL) $$i $(DESTDIR)$(ext_dir) ; \
	done

uninstall-local:
	for i in $(plugins); do \
	  $(LIBTOOL) --mode=uninstall $(RM) $(DESTDIR)$(ext_dir)/$$i ; \
	done

check-local: all-local
