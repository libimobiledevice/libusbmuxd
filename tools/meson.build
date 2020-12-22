iproxy = executable('iproxy',
  'iproxy.c',
  include_directories : libusbmuxd_incdir,
  link_with : libusbmuxd,
  dependencies : thread_dep,
)

inetcat = executable('inetcat',
  'inetcat.c',
  include_directories : libusbmuxd_incdir,
  link_with : libusbmuxd,
)
