AUTOMAKE_OPTIONS = dist-bzip2

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS=\
	gegl-gtk \
	operations \
	examples \
	tests

pkgconfigdir = $(libdir)/pkgconfig

if HAVE_GTK2
pkgconfig_DATA = gegl-gtk2-0.1.pc
else
pkgconfig_DATA = gegl-gtk3-0.1.pc
endif

EXTRA_DIST = \
	autogen.sh \
	docs/reference/gegl-gtk-docs.xml.in \
	docs/gtk-doc.make \
	gegl-gtk2-0.1.pc.in \
	gegl-gtk3-0.1.pc.in \
	m4/introspection.m4

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection --enable-gtk-doc

DISTCLEANFILES =

# Documentation
gtkdoc_builddir=$(top_builddir)/docs/reference/
gtkdoc_distdir=$(top_distdir)/docs/reference/
gtkdoc_srcdir=$(top_srcdir)/docs/reference/

all-local: $(noinst_LTLIBRARIES)
	cd $(gtkdoc_builddir) && $(MAKE) $(AM_MAKEFLAGS) all

clean-local:
	cd $(gtkdoc_builddir) && $(MAKE) $(AM_MAKEFLAGS) clean
	rm -f src/*.stamp src/*.tmp

distclean-local:
	cd $(gtkdoc_builddir) && $(MAKE) $(AM_MAKEFLAGS) distclean

docs: $(noinst_LTLIBRARIES)
	cd $(gtkdoc_builddir) && $(MAKE) $(AM_MAKEFLAGS) docs

install-data-local:
	cd $(gtkdoc_builddir) && $(MAKE) $(AM_MAKEFLAGS) install-data

uninstall-local:
	cd $(gtkdoc_builddir) && $(MAKE) $(AM_MAKEFLAGS) uninstall

dist-hook:
	mkdir -p $(gtkdoc_distdir)
	cp $(gtkdoc_srcdir)/Makefile.am $(gtkdoc_srcdir)/Makefile.in $(gtkdoc_distdir)
	cd $(gtkdoc_builddir) && $(MAKE) $(AM_MAKEFLAGS) \
	distdir="../../$(gtkdoc_distdir)" \
	top_distdir="../../$(top_distdir)" \
	dist-hook

.PHONY: docs
# End Documentation
