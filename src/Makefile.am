AM_CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/common

AM_CFLAGS = $(GLOBAL_CFLAGS) $(libplist_CFLAGS)
AM_LDFLAGS = $(GLOBAL_LIBS) $(libpthread_LIBS) $(libplist_LIBS)

lib_LTLIBRARIES = libusbmuxd.la
libusbmuxd_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LIBUSBMUXD_SO_VERSION) -no-undefined
libusbmuxd_la_LIBADD = $(top_builddir)/common/libinternalcommon.la
libusbmuxd_la_SOURCES = libusbmuxd.c

if WIN32
libusbmuxd_la_LDFLAGS += -avoid-version
libusbmuxd_la_LIBADD += -lws2_32
endif
