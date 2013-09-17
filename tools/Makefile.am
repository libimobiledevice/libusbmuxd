AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir)/src
AM_LDFLAGS =

bin_PROGRAMS = iproxy

iproxy_SOURCES = iproxy.c
iproxy_CFLAGS = $(AM_CFLAGS)
iproxy_LDFLAGS = $(AM_LDFLAGS)
iproxy_LDADD = ../src/libusbmuxd.la

