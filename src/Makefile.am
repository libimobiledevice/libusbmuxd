AM_CPPFLAGS = -I$(top_srcdir)/include

AM_CFLAGS = $(GLOBAL_CFLAGS) $(libplist_CFLAGS) $(limd_glue_CFLAGS)
AM_LDFLAGS = $(GLOBAL_LIBS) $(libplist_LIBS) $(limd_glue_LIBS)

lib_LTLIBRARIES = libusbmuxd-2.0.la
libusbmuxd_2_0_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LIBUSBMUXD_SO_VERSION) -no-undefined
libusbmuxd_2_0_la_LIBADD =
libusbmuxd_2_0_la_SOURCES = libusbmuxd.c

if WIN32
libusbmuxd_2_0_la_LDFLAGS += -avoid-version -static-libgcc
libusbmuxd_2_0_la_LIBADD += -lws2_32 -lIphlpapi
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libusbmuxd-2.0.pc
