AUTOMAKE_OPTIONS = foreign
SUBDIRS = compat doc include src man wiced

AM_CPPFLAGS = @STRIP_BEGIN@ \
	      -I$(top_srcdir)/include \
	      -I$(top_builddir)/include \
	      @STRIP_END@

if DEBUG
AM_CFLAGS = -Wall -g -O0 -DDEBUG
AM_CXXFLAGS = -Wall -g -O0 -DDEBUG
else
AM_CFLAGS = -Wall -O3
AM_CXXFLAGS = -Wall -O3
endif

EXTRA_DIST = README.wiced wiced.mk
