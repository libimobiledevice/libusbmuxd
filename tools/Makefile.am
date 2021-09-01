AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir)/src -I$(top_srcdir)/include $(limd_glue_CFLAGS)
AM_LDFLAGS = $(libpthread_LIBS) $(limd_glue_LIBS)

bin_PROGRAMS = iproxy inetcat

iproxy_SOURCES = iproxy.c
iproxy_CFLAGS = $(AM_CFLAGS)
iproxy_LDFLAGS = $(AM_LDFLAGS)
iproxy_LDADD = $(top_builddir)/src/libusbmuxd-2.0.la

inetcat_SOURCES = inetcat.c
inetcat_CFLAGS = $(AM_CFLAGS)
inetcat_LDFLAGS = $(AM_LDFLAGS)
inetcat_LDADD = $(top_builddir)/src/libusbmuxd-2.0.la

