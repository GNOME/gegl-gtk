
check_PROGRAMS = test-view test-view-helper

test_view_SOURCES = test-view.c
test_view_CPPFLAGS = $(GTK_CFLAGS) $(GEGL_CFLAGS) -I$(top_srcdir)/gegl-gtk
test_view_LDADD = $(top_builddir)/gegl-gtk/libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la $(GTK_LIBS) $(GEGL_LIBS)

test_view_helper_SOURCES = test-view-helper.c
test_view_helper_CPPFLAGS = $(GTK_CFLAGS) $(GEGL_CFLAGS) -I$(top_srcdir)/gegl-gtk
test_view_helper_LDADD = $(top_builddir)/gegl-gtk/libgegl-gtk@GEGL_GTK_GTK_VERSION@-@GEGL_GTK_API_VERSION@.la $(GTK_LIBS) $(GEGL_LIBS)

EXTRA_DIST = utils.c

# ----------------------------------------------
# Rules for hooking up the unit/functional tests

TESTS_ENVIRONMENT = 

# Run tests with xvfb-run if available
if HAVE_XVFB_RUN
TESTS_ENVIRONMENT += $(XVFB_RUN) --server-args="-screen 0 1280x1024x24"
endif

GTESTER = $(TESTS_ENVIRONMENT) gtester -k
GTESTER_REPORT = gtester-report

# test: run all tests
test: ${check_PROGRAMS}
	@test -z "${check_PROGRAMS}" || top_srcdir=${top_srcdir} ${GTESTER} --verbose ${check_PROGRAMS}

# test-report: run tests and generate report
# perf-report: run tests with -m perf and generate report
# full-report: like test-report: with -m perf and -m slow
test-report perf-report full-report: ${check_PROGRAMS}
	@ignore_logdir=true; \
	if test -z "$$GTESTER_LOGDIR"; then \
	  GTESTER_LOGDIR=`mktemp -d "\`pwd\`/.testlogs-XXXXXX"`; export GTESTER_LOGDIR; \
	  ignore_logdir=false; \
	fi; \
	test -z "${check_PROGRAMS}" || { \
	  case $@ in \
	  test-report) test_options="-k";; \
	  perf-report) test_options="-k -m=perf";; \
	  full-report) test_options="-k -m=perf -m=slow";; \
	  esac; \
	  if test -z "$$GTESTER_LOGDIR"; then	\
	    top_srcdir=${top_srcdir} ${GTESTER} --verbose $$test_options -o test-report.xml ${check_PROGRAMS}; \
	  elif test -n "${check_PROGRAMS}"; then \
	    top_srcdir=${top_srcdir} ${GTESTER} --verbose $$test_options -o `mktemp "$$GTESTER_LOGDIR/log-XXXXXX"` ${check_PROGRAMS}; \
	  fi; \
	}; \
	$$ignore_logdir || { \
	  echo '<?xml version="1.0"?>' > $@.xml; \
	  echo '<report-collection>'  >> $@.xml; \
	  for lf in `ls -L "$$GTESTER_LOGDIR"/.`; do \
	    sed '1,1s/^<?xml\b[^>?]*?>//' <"$$GTESTER_LOGDIR"/"$$lf" >> $@.xml; \
	  done; \
	  echo >> $@.xml; \
	  echo '</report-collection>' >> $@.xml; \
	  rm -rf "$$GTESTER_LOGDIR"/; \
	  ${GTESTER_REPORT} --version 2>/dev/null 1>&2; test "$$?" != 0 || ${GTESTER_REPORT} $@.xml >$@.html; \
	}
# run make test as part of make check
check-local: test

.PHONY: test test-report perf-report full-report
