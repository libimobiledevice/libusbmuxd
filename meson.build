project('libusbmuxd', 'c',
  version : '2.0.3',
  default_options : 'c_std=c99')

cc = meson.get_compiler('c')
cdata = configuration_data()

cdata.set_quoted('PACKAGE_NAME', meson.project_name())
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())
cdata.set_quoted('PACKAGE_STRING', meson.project_name() + ' ' + meson.project_version())
cdata.set_quoted('PACKAGE_URL', 'https://libimobiledevice.org')
cdata.set_quoted('PACKAGE_BUGREPORT', 'https://libimobiledevice.org')

cdata.set('HAVE_GETIFADDRS', cc.has_function('getifaddrs', prefix: '#include <ifaddrs.h>'))
cdata.set('HAVE_INOTIFY', cc.has_function('inotify_init', prefix: '#include <sys/inotify.h>'))
cdata.set('HAVE_PROGRAM_INVOCATION_SHORT_NAME', cc.has_function('program_invocation_short_name', prefix: '#define _GNU_SOURCE\n#include <errno.h>'))
cdata.set('HAVE_PSELECT', cc.has_function('pselect', prefix: '#define _GNU_SOURCE\n#include <sys/select.h>'))
cdata.set('HAVE_REALLOC', cc.has_function('realloc', prefix: '#include <stdlib.h>'))
cdata.set('HAVE_SLEEP', cc.has_function('sleep', prefix: '#include <unistd.h>'))
cdata.set('HAVE_STPNCPY', cc.has_function('stpncpy', prefix: '#define _GNU_SOURCE\n#include <string.h>'))
cdata.set('HAVE_STRCASECMP', cc.has_function('strcasecmp', prefix: '#include <strings.h>'))
cdata.set('HAVE_STRDUP', cc.has_function('strdup', prefix: '#define _GNU_SOURCE\n#include <string.h>'))
cdata.set('HAVE_STRERROR', cc.has_function('strerror', prefix: '#include <string.h>'))

cdata.set('HAVE_HAVE_PROGRAM_INVOCATION_SHORT_NAME_ERRNO_H', cc.has_header_symbol('errno.h', 'program_invocation_short_name'))
cdata.set('HAVE_PTHREAD_CANCEL', cc.has_header_symbol('pthread.h', 'pthread_cancel'))

configure_file(input : 'config.h.meson',
  output : 'config.h',
  configuration : cdata)

libusbmuxd_incdir = include_directories(['include', 'common', '.'])

add_global_arguments(['-DHAVE_CONFIG_H', '-D_GNU_SOURCE'], language : 'c')

thread_dep = dependency('threads')

install_headers(['include/usbmuxd.h', 'include/usbmuxd-proto.h'])

subdir('common')
subdir('src')
if get_option('tools')
  subdir('tools')
endif
