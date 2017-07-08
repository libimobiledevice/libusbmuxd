AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir)/common -I$(top_srcdir)/src -I$(top_srcdir)/include
AM_LDFLAGS = $(libpthread_LIBS)

bin_PROGRAMS = iproxy usbmuxinfo usbmuxscan

iproxy_SOURCES = iproxy.c
iproxy_CFLAGS = $(AM_CFLAGS)
iproxy_LDFLAGS = $(AM_LDFLAGS)
iproxy_LDADD = $(top_builddir)/src/libusbmuxd.la $(top_builddir)/common/libinternalcommon.la

usbmuxinfo_SOURCES = usbmuxinfo.c
usbmuxinfo_CFLAGS = $(AM_CFLAGS)
usbmuxinfo_LDFLAGS = $(AM_LDFLAGS)
usbmuxinfo_LDADD = $(top_builddir)/src/libusbmuxd.la $(top_builddir)/common/libinternalcommon.la

usbmuxscan_SOURCES = usbmuxscan.c
usbmuxscan_CFLAGS = $(AM_CFLAGS)
usbmuxscan_LDFLAGS = $(AM_LDFLAGS)
usbmuxscan_LDADD = $(top_builddir)/src/libusbmuxd.la $(top_builddir)/common/libinternalcommon.la
