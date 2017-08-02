AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir)/common -I$(top_srcdir)/src -I$(top_srcdir)/include
AM_LDFLAGS = $(libpthread_LIBS)

bin_PROGRAMS = iproxy icat

iproxy_SOURCES = iproxy.c
iproxy_CFLAGS = $(AM_CFLAGS)
iproxy_LDFLAGS = $(AM_LDFLAGS)
iproxy_LDADD = $(top_builddir)/src/libusbmuxd.la $(top_builddir)/common/libinternalcommon.la

icat_SOURCES = icat.c
icat_CFLAGS = $(AM_CFLAGS)
icat_LDFLAGS = $(AM_LDFLAGS)
icat_LDADD = $(top_builddir)/src/libusbmuxd.la $(top_builddir)/common/libinternalcommon.la

