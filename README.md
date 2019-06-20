# libusbmuxd

## About

A client library to multiplex connections from and to iOS devices by connecting
to a socket provided by a usbmuxd daemon.

## Requirements

Development Packages of:
* libplist

Software:
* usbmuxd (Windows and Mac OS X can use the one provided by iTunes)
* make
* autoheader
* automake
* autoconf
* libtool
* pkg-config
* gcc or clang

Optional:
* inotify (Linux only)

## Installation

To compile run:
```bash
./autogen.sh
make
sudo make install
```

If dependent packages cannot be found when running autogen.sh (or configure)
make sure that `pkg-config` is installed and set the `PKG_CONFIG_PATH` environment
variable if required. It can be passed directly like this:

```bash
PKG_CONFIG_PATH=/usr/local/lib/pkgconfig ./autogen.sh
```

If you require a custom prefix or other option being passed to `./configure`
you can pass them directly to `./autogen.sh` like this:
```bash
./autogen.sh --prefix=/opt/local --without-cython
make
sudo make install
```

## Who/What/Where?

* Home: https://www.libimobiledevice.org/
* Code: `git clone https://git.libimobiledevice.org/libusbmuxd.git`
* Code (Mirror): `git clone https://github.com/libimobiledevice/libusbmuxd.git`
* Tickets: https://github.com/libimobiledevice/libusbmuxd/issues
* Mailing List: https://lists.libimobiledevice.org/mailman/listinfo/libimobiledevice-devel
* IRC: irc://irc.freenode.net#libimobiledevice

## Credits

Apple, iPhone, iPod, and iPod Touch are trademarks of Apple Inc.
libimobiledevice is an independent software library and has not been
authorized, sponsored, or otherwise approved by Apple Inc.

README Updated on: 2019-06-20
