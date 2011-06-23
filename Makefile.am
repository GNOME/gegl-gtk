AUTOMAKE_OPTIONS = dist-bzip2

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS=\
	gegl-gtk \
	operations \
	examples

pkgconfigdir = $(libdir)/pkgconfig

if HAVE_GTK2
pkgconfig_DATA = gegl-gtk2-0.1.pc
else
pkgconfig_DATA = gegl-gtk3-0.1.pc
endif

EXTRA_DIST = \
	$(built_dist_files) \
	autogen.sh \
	gegl-gtk2-0.1.pc.in \
	gegl-gtk3-0.1.pc.in \
	m4/introspection.m4

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

DISTCLEANFILES = \
	$(built_dist_files)
