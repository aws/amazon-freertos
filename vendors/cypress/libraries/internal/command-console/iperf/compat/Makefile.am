noinst_LIBRARIES = libcompat.a

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

libcompat_a_SOURCES = \
		      Thread.c \
		      error.c \
		      delay.cpp \
		      gettimeofday.c \
		      headers_slim.h \
		      inet_ntop.c \
		      inet_pton.c \
		      signal.c \
		      snprintf.c \
		      string.c \
		      setitimer.c

