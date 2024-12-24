pkg = import('pkgconfig')
libplist_dep = dependency('libplist-2.0')
thread_dep = dependency('threads')

libusbmuxd = library('usbmuxd',
  common_files,
  'libusbmuxd.c',
  include_directories : libusbmuxd_incdir,
  dependencies : [ libplist_dep, thread_dep ],
  install : true,
)

pkg.generate(
  libraries : libusbmuxd,
  version: meson.project_version(),
  name : 'libusbmuxd-2.0',
  description : 'A library to communicate with the usbmux daemon',
)
