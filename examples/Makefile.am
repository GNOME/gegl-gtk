
CFILES = $(wildcard $(srcdir)/*.c)
bins   = $(subst $(srcdir)/,,$(CFILES:.c=))
EXTRA_DIST = $(wildcard *.c) $(wildcard util/*.[ch])

gegl_lt_cc_v = $(gegl_lt_cc_v_$(V))
gegl_lt_cc_v_ = $(gegl_lt_cc_v_$(AM_DEFAULT_VERBOSITY))
gegl_lt_cc_v_0 = @echo "  CC     " $<;
gegl_lt_ccld_v = $(gegl_lt_ccld_v_$(V))
gegl_lt_ccld_v_ = $(gegl_lt_ccld_v_$(AM_DEFAULT_VERBOSITY))
gegl_lt_ccld_v_0 = @echo "  CCLD   " $@;

AM_CPPFLAGS = \
	-I$(top_builddir)/gegl-gtk

AM_CFLAGS = $(GEGL_CFLAGS) $(GTK_CFLAGS)

AM_LDFLAGS =  \
	$(top_builddir)/gegl-gtk/libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la \
	$(GEGL_LIBS) $(GTK_LIBS)

all-local: $(bins)

%: %.c $(GEGLHEADERS)
	$(gegl_lt_cc_v) $(LIBTOOL) --quiet --mode=compile $(CC) $(DEFS) -I. -I$(top_builddir) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS) -c -o $@.lo $< 
	$(gegl_lt_ccld_v) $(LIBTOOL) --quiet --mode=link $(CC) $(AM_LDFLAGS) $(LDFLAGS) $(CFLAGS) -o $@ $@.lo $(LIBS)

clean-local:
	rm -f *.lo $(bins)


