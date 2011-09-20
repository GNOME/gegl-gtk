dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.54)

dnl Making releases on the stable branch:
dnl   GEGL_GTK_MICRO_VERSION += 1;
dnl   GEGL_GTK_INTERFACE_AGE += 1;
dnl   GEGL_GTK_BINARY_AGE += 1;
dnl if any functions have been added,
dnl    set GEGL_GTK_INTERFACE_AGE to 0.
dnl if backwards compatibility has been broken,
dnl    set GEGL_GTK_BINARY_AGE _and_ GEGL_GTK_INTERFACE_AGE to 0.

m4_define([gegl_gtk_major_version], [0])
m4_define([gegl_gtk_minor_version], [0])
m4_define([gegl_gtk_micro_version], [7])
m4_define([gegl_gtk_api_version], [0.1])

m4_define([gegl_gtk_real_version],
          [gegl_gtk_major_version.gegl_gtk_minor_version.gegl_gtk_micro_version])
m4_define([gegl_gtk_version], [gegl_gtk_real_version])

dnl Number of releases since we've added interfaces
m4_define([gegl_gtk_interface_age], [1])

m4_define([gegl_gtk_binary_age],
          [m4_eval(100 * gegl_gtk_minor_version + gegl_gtk_micro_version)])

m4_define([stability_version_number],
          m4_if(m4_eval(gegl_gtk_major_version || gegl_gtk_minor_version), [0], 
                        [gegl_gtk_micro_version], [gegl_gtk_minor_version]))
m4_define([gegl_gtk_unstable],
          m4_if(m4_eval(stability_version_number % 2), [1], [yes], [no]))
m4_define([gegl_gtk_stable],
          m4_if(m4_eval(stability_version_number % 2), [0], [yes], [no]))

# required versions of external libraries
m4_define([babl_required_version], [0.1.4])
m4_define([gegl_required_version], [0.1.6])
m4_define([glib_required_version], [2.22.0])
m4_define([gtk2_required_version], [2.18.0])
m4_define([gtk3_required_version], [3.0.0])

AC_INIT(gegl-gtk, gegl_gtk_major_version.gegl_gtk_minor_version.gegl_gtk_micro_version)
#AC_CONFIG_SRCDIR([gegl/gegl.h])
AC_CONFIG_MACRO_DIR([m4])

# Enable silent build rules by default, requires at least
# Automake-1.11. Disable by either passing --disable-silent-rules to
# configure or passing V=1 to make
m4_ifdef([AM_SILENT_RULES],
         [AM_SILENT_RULES([yes])],
         [AC_SUBST([AM_DEFAULT_VERBOSITY], [1])])

dnl ==========================================================================
dnl                              Versioning              
dnl ==========================================================================

dnl The triplet 
GEGL_GTK_MAJOR_VERSION=gegl_gtk_major_version
GEGL_GTK_MINOR_VERSION=gegl_gtk_minor_version
GEGL_GTK_MICRO_VERSION=gegl_gtk_micro_version
GEGL_GTK_INTERFACE_AGE=gegl_gtk_interface_age
GEGL_GTK_BINARY_AGE=gegl_gtk_binary_age
GEGL_GTK_VERSION=gegl_gtk_version
GEGL_GTK_REAL_VERSION=gegl_gtk_real_version
GEGL_GTK_VERSION=gegl_gtk_version
GEGL_GTK_API_VERSION=gegl_gtk_api_version

AC_SUBST(GEGL_GTK_MAJOR_VERSION)
AC_SUBST(GEGL_GTK_MINOR_VERSION)
AC_SUBST(GEGL_GTK_MICRO_VERSION)
AC_SUBST(GEGL_GTK_INTERFACE_AGE)
AC_SUBST(GEGL_GTK_BINARY_AGE)
AC_SUBST(GEGL_GTK_VERSION)
AC_SUBST(GEGL_GTK_REAL_VERSION)
AC_SUBST(GEGL_GTK_API_VERSION)

dnl The symbol GEGL_GTK_UNSTABLE is defined above for substitution in
dnl Makefiles and conditionally defined here as a preprocessor symbol
dnl and automake conditional.
GEGL_GTK_UNSTABLE=gegl_gtk_unstable
if test "x$GEGL_GTK_UNSTABLE" = "xyes"; then
  AC_DEFINE(GEGL_GTK_UNSTABLE, 1,
            [Define to 1 if this is an unstable version of GEGL])
fi
AM_CONDITIONAL(GEGL_GTK_UNSTABLE, test "x$GEGL_GTK_UNSTABLE" = "xyes")


dnl libtool versioning
m4_define([lt_current], [m4_eval(100 * gegl_gtk_minor_version + gegl_gtk_micro_version - gegl_gtk_interface_age)])
m4_define([lt_revision], [gegl_gtk_interface_age])
m4_define([lt_age], [m4_eval(gegl_gtk_binary_age - gegl_gtk_interface_age)])
GEGL_GTK_LIBRARY_VERSION="lt_current:lt_revision:lt_age"
GEGL_GTK_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(GEGL_GTK_LIBRARY_VERSION)
AC_SUBST(GEGL_GTK_CURRENT_MINUS_AGE)

GLIB_REQUIRED_VERSION=glib_required_version
GTK2_REQUIRED_VERSION=gtk2_required_version
GTK3_REQUIRED_VERSION=gtk3_required_version
AC_SUBST(GLIB_REQUIRED_VERSION)
AC_SUBST(GTK2_REQUIRED_VERSION)
AC_SUBST(GTK3_REQUIRED_VERSION)

PACKAGE_NAME=gegl-gtk
AC_SUBST(PACKAGE_NAME)

GEGL_PLUGINS_DIR=`pkg-config gegl --variable=pluginsdir`
if test -z $GEGL_PLUGINS_DIR; then
	GEGL_PREFIX=`pkg-config gegl --variable=prefix`
	GEGL_PLUGINS_DIR=$GEGL_PREFIX/lib/gegl-0.1
fi
AC_SUBST(GEGL_PLUGINS_DIR)

dnl ==========================================================================

AM_INIT_AUTOMAKE(no-define no-dist-gzip foreign)
AM_CONFIG_HEADER(config.h)

GEGL_GTK_SERVER=www.gegl.org
AC_SUBST(GEGL_GTK_SERVER)

dnl Initialize libtool
AC_LIBTOOL_DLOPEN
AC_PROG_CC
AC_PROG_CXX
AM_DISABLE_STATIC
AM_PROG_LIBTOOL

dnl Initialize maintainer mode
AM_MAINTAINER_MODE


###############
# Generic stuff
###############

dnl Checks for programs.
AC_ISC_POSIX
#AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Large file support.
AC_SYS_LARGEFILE

AC_MSG_CHECKING([whether to turn on debugging])
AC_ARG_ENABLE(debug,
              [  --enable-debug          turn on  debugging (default=no)],
              if eval "test x$enable_debug = xyes"; then
                DEBUGFLAG="-g -DGEGL_GTK_ENABLE_DEBUG"
              fi)

if test -n "$DEBUGFLAG"; then
  AC_MSG_RESULT([yes])
  CFLAGS="$DEBUGFLAG $CFLAGS"
else
  AC_MSG_RESULT([no])
fi

AC_MSG_CHECKING([whether to turn on profiling])
AC_ARG_ENABLE(profile,
              [  --enable-profile        turn on profiling (default=no)],
              if eval "test x$enable_profile = xyes"; then
                PROFFLAG="-pg"
              fi)

if test -n "$PROFFLAG"; then
  AC_MSG_RESULT([yes])
  CFLAGS="$PROFFLAG $CFLAGS"
else
  AC_MSG_RESULT([no])
fi

AC_C_RESTRICT


######################################
# Checks for BABL
######################################
PKG_CHECK_MODULES(BABL, babl >= babl_required_version)

######################################
# Checks for GEGL
######################################
PKG_CHECK_MODULES(GEGL, gegl-0.2 >= gegl_required_version)

######################################
# Checks for required deps
######################################
AC_MSG_CHECKING([which gtk+ version to compile against])
AC_ARG_WITH([gtk],
	[AS_HELP_STRING([--with-gtk=2.0|3.0],[which gtk+ version to compile against (default: 2.0)])],
	[case "$with_gtk" in
	2.0|3.0) ;;
	*) AC_MSG_ERROR([invalid gtk version specified]) ;;
	esac],
	[with_gtk=2.0])
	AC_MSG_RESULT([$with_gtk])

have_gtk="no"
case "$with_gtk" in
	2.0) PKG_CHECK_MODULES(GTK, gtk+-2.0 >= gtk2_required_version,
		[have_gtk="2.0" GEGL_GTK_GTK_VERSION="2"
		AC_DEFINE(HAVE_GTK2, 1, [Define to 1 to compile for gtk2])],
		[]) ;;

	3.0) PKG_CHECK_MODULES(GTK, gtk+-3.0 >= gtk3_required_version,
		[have_gtk="3.0" GEGL_GTK_GTK_VERSION="3"
		AC_DEFINE(HAVE_GTK3, 1, [Define to 1 to compile for gtk3])],
		[]) ;;
	*) ;;
esac

m4_define([gegl_gtk_gtk_version], [$GEGL_GTK_GTK_VERSION])

# Files with versions in their names
AC_CONFIG_FILES(
gegl-gtk/GeglGtk\
gegl_gtk_gtk_version-gegl_gtk_api_version.metadata:gegl-gtk/GeglGtk.metadata.in
)

AC_SUBST(GEGL_GTK_GTK_VERSION)

AM_CONDITIONAL(HAVE_GTK2, test "$have_gtk" = "2.0")
AM_CONDITIONAL(HAVE_GTK3, test "$have_gtk" = "3.0")
	
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

##################
# Cairo gobject
###################
AC_ARG_WITH(cairo-gobject, [  --without-cairo-gobject    build without Cairo GObject support])

have_cairo_gobject="no"
if test "x$with_cairo_gobject" != "xno"; then
  PKG_CHECK_MODULES(CAIRO_GOBJECT, cairo-gobject,
    [have_cairo_gobject="yes" AC_DEFINE(HAVE_CAIRO_GOBJECT, 1, [Define to 1 to compile for gtk2])],
    have_cairo_gobject="no  (Cairo GObject not found)")
fi

AM_CONDITIONAL(HAVE_CAIRO_GOBJECT, test "$have_cairo_gobject" = "yes")

AC_SUBST(CAIRO_GOBJECT_CFLAGS)
AC_SUBST(CAIRO_GOBJECT_LIBS)

########################
# Check GObject Introspection
########################
AC_ARG_ENABLE([introspection],
              [  --disable-introspection        disable GObject introspection support (default=no)],,
              enable_introspection="yes")

if test "x$enable_introspection" = "xyes"; then
	GOBJECT_INTROSPECTION_REQUIRE([0.9.5]) # Will abort on failure
fi

AM_CONDITIONAL(HAVE_INTROSPECTION, test "x$enable_introspection" = "xyes")

if test "x$GIRDIR" = "x"; then
  GIRDIR=$(datadir)/vala/vapi
fi
AC_SUBST([GIRDIR])

########################
# Check Vala
########################

AC_ARG_WITH(vala, [  --without-vala         build without Vala support])

have_vapigen="no"
if test "x$with_vala" != "xno"; then

  AC_PATH_PROG(VAPIGEN, vapigen, no)
  if test "$VAPIGEN" = "no"; then
    have_vapigen="no  (vapigen executable not found)"
    AC_MSG_RESULT([*** Check for vapigen failed.])
  else
    have_vapigen="yes"
  fi
fi

have_vala=$have_vapigen
AM_CONDITIONAL(HAVE_VALA, test "$have_vala" = "yes")

if test "x$VAPIDIR" = "x"; then
  VAPIDIR=$(datadir)/vala/vapi
fi
AC_SUBST([VAPIDIR])

# `$PKG_CONFIG --variable=vapidir $valapkg`

######################################
# Checks for gtk-doc and docbook-tools
######################################

GTK_DOC_CHECK([1.0], [--docdir docs --flavour no-tmpl])

dnl NOTE: We need to use a separate automake conditional for this
dnl   	  to make this work with the tarballs.
AM_CONDITIONAL(ENABLE_GTK_DOC, test "x$enable_gtk_doc" = "xyes")

AC_CHECK_PROGS(PERL, perl5 perl)


AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "gegl-gtk-$GEGL_GTK_API_VERSION",
                   [Our gettext translation domain.])

####################
# Check for xvfb-run
####################

AC_ARG_WITH(xvfb-run,  [  --without-xvfb-run      Do not use xvfb-run for UI-dependent automatic tests])

have_xvfb_run="no (disabled)"
if test "x$with_xvfb_run" != "xno"; then
  AC_PATH_PROG(XVFB_RUN, xvfb-run, no)
  if test "x$XVFB_RUN" != "xno"; then
    have_xvfb_run="yes"
  else
    have_xvfb_run="no (not found)"
  fi
fi
AM_CONDITIONAL(HAVE_XVFB_RUN, test "x$have_xvfb_run" == "xyes")

#######################
# Enable extra warnings
#######################

DESIRED_CFLAGS="-Wall -Wdeclaration-after-statement -Wmissing-prototypes -Wmissing-declarations -Winit-self -Wpointer-arith -Wold-style-definition"

#for flag in $DESIRED_CFLAGS; do
#   AS_COMPILER_FLAG([$flag], [CFLAGS="$CFLAGS $flag"])
#done

dnl Disable deprecated features in maintainer mode
if test -z "${MAINTAINER_MODE_TRUE}"; then
   GLIB_CFLAGS="-DG_DISABLE_DEPRECATED $GLIB_CFLAGS"
   GTK_CFLAGS="-DG_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED $GTK_CFLAGS"
fi

AC_OUTPUT([
Makefile
gegl-gtk/Makefile
operations/Makefile
examples/Makefile
examples/c/Makefile
examples/vala/Makefile
tests/Makefile
docs/reference/Makefile
docs/reference/gegl-gtk-docs.xml
gegl-gtk2-0.1.pc
gegl-gtk3-0.1.pc
])

dnl Print a summary of features enabled/disabled:
AC_MSG_RESULT([
Building GEGL with prefix=$prefix

  GTK+ version:           $have_gtk
  Documentation:          $enable_gtk_doc
  Cairo GObject: 	  $have_cairo_gobject
  GObject Introspection:  $enable_introspection
  Vala support:           $have_vala
]);
