# libusbmuxd

*A client library for applications to handle usbmux protocol connections with iOS devices.*

![](https://github.com/libimobiledevice/libusbmuxd/actions/workflows/build.yml/badge.svg)

## Table of Contents
- [Features](#features)
- [Building](#building)
  - [Prerequisites](#prerequisites)
    - [Linux (Debian/Ubuntu based)](#linux-debianubuntu-based)
    - [macOS](#macos)
    - [Windows](#windows)
  - [Configuring the source tree](#configuring-the-source-tree)
  - [Building and installation](#building-and-installation)
- [Usage](#usage)
- [Contributing](#contributing)
- [Links](#links)
- [License](#license)
- [Credits](#credits)

## Overview

This project is a client library to multiplex connections from and to iOS
devices alongside command-line utilities.

It is primarily used by applications which use the [libimobiledevice](https://github.com/libimobiledevice/)
library to communicate with services running on iOS devices.

The library does not establish a direct connection with a device but requires
connecting to a socket provided by the `usbmuxd` daemon.

On Windows and Mac OS X, the `usbmuxd` daemon is provided by iTunes upon installing. For Linux, the [libimobiledevice project](https://github.com/libimobiledevice/) provides an open-source reimplementation of the [usbmuxd daemon](https://github.com/libimobiledevice/usbmuxd.git/) to use on Linux or as an alternative to communicate with iOS devices.

## Key features

- **Protocol:** Provides an interface to handle the usbmux protocol
- **Port Proxy:** Provides the `iproxy` utility to proxy ports to the device
- **Netcat:** Provides the `inetcat` utility to expose a raw connection to the device
- **Cross-Platform:** Tested on Linux, macOS, Windows and Android platforms
- **Flexible:** Allows using the open-source or proprietary usbmuxd daemon

Furthermore the Linux build optionally provides support using inotify if
available.

## Building

### Prerequisites

You need to have a working compiler (gcc/clang) and development environent
available. This project uses autotools for the build process, allowing to
have common build steps across different platforms.
Only the prerequisites differ and they are described in this section.

`libusbmuxd` requires [libplist](https://github.com/libimobiledevice/libplist) and [libimobiledevice-glue](https://github.com/libimobiledevice/libimobiledevice-glue).

- On Linux it also requires [usbmuxd](https://github.com/libimobiledevice/usbmuxd) to be installed and running on the system.
- On macOS this service is already provided by the operating system.
- On Windows [the Apple Mobile Device Service](https://support.apple.com/102347) is automatically provided when iTunes is installed.
Check [libplist's Building](https://github.com/libimobiledevice/libplist?tab=readme-ov-file#building) and [libimobiledevice-glue's Building](https://github.com/libimobiledevice/libimobiledevice-glue?tab=readme-ov-file#building)
section of the respective README on how to build them. Note that some platforms might have them as a package.

#### Linux (Debian/Ubuntu based)

* Install all required dependencies and build tools:
  ```shell
  sudo apt-get install \
  	build-essential \
  	pkg-config \
  	checkinstall \
  	git \
  	autoconf \
  	automake \
  	libtool-bin \
  	libplist-dev \
  	libimobiledevice-glue-dev \
  	usbmuxd
  ```
  In case `libplist-dev`, `libimobiledevice-glue-dev`, or `usbmuxd` are not available, you can manually build and install them. See note above.

#### macOS

* Make sure the Xcode command line tools are installed. Then, use either [MacPorts](https://www.macports.org/)
  or [Homebrew](https://brew.sh/) to install `automake`, `autoconf`, `libtool`, etc.

  Using MacPorts:
  ```shell
  sudo port install libtool autoconf automake pkgconfig
  ```

  Using Homebrew:
  ```shell
  brew install libtool autoconf automake pkg-config
  ```

#### Windows

* Make sure iTunes is installed, which provides the `usbmuxd` service.
* Using [MSYS2](https://www.msys2.org/) is the official way of compiling this project on Windows. Download the MSYS2 installer
  and follow the installation steps.

  It is recommended to use the _MSYS2 MinGW 64-bit_ shell. Run it and make sure the required dependencies are installed:

  ```shell
  pacman -S base-devel \
  	git \
  	mingw-w64-x86_64-gcc \
  	make \
  	libtool \
  	autoconf \
  	automake-wrapper \
  	pkg-config
  ```
  NOTE: You can use a different shell and different compiler according to your needs. Adapt the above command accordingly.

### Configuring the source tree

You can build the source code from a git checkout, or from a `.tar.bz2` release tarball from [Releases](https://github.com/libimobiledevice/libusbmuxd/releases).
Before we can build it, the source tree has to be configured for building. The steps depend on where you got the source from.

Since libusbmuxd depends on other packages, you should set the pkg-config environment variable `PKG_CONFIG_PATH`
accordingly. Make sure to use a path with the same prefix as the dependencies. If they are installed in `/usr/local` you would do

```shell
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig
```

* **From git**

  If you haven't done already, clone the actual project repository and change into the directory.
  ```shell
  git clone https://github.com/libimobiledevice/libusbmuxd
  cd libusbmuxd
  ```

  Configure the source tree for building:
  ```shell
  ./autogen.sh
  ```

* **From release tarball (.tar.bz2)**

  When using an official [release tarball](https://github.com/libimobiledevice/libusbmuxd/releases) (`libusbmuxd-x.y.z.tar.bz2`)
  the procedure is slightly different.

  Extract the tarball:
  ```shell
  tar xjf libusbmuxd-x.y.z.tar.bz2
  cd libusbmuxd-x.y.z
  ```

  Configure the source tree for building:
  ```shell
  ./configure
  ```

Both `./configure` and `./autogen.sh` (which generates and calls `configure`) accept a few options, for example `--prefix` to allow
building for a different target folder. You can simply pass them like this:

```shell
./autogen.sh --prefix=/usr/local
```
or
```shell
./configure --prefix=/usr/local
```

Once the command is successful, the last few lines of output will look like this:
```
[...]
config.status: creating config.h
config.status: executing depfiles commands
config.status: executing libtool commands

Configuration for libusbmuxd 2.1.0:
-------------------------------------------

  Install prefix: .........: /usr/local
  inotify support (Linux) .: no

  Now type 'make' to build libusbmuxd 2.1.0,
  and then 'make install' for installation.
```

## Usage

### iproxy

This utility allows binding local TCP ports so that a connection to one (or
more) of the local ports will be forwarded to the specified port (or ports) on
a usbmux device.

Bind local TCP port 2222 and forward to port 22 of the first device connected
via USB:
```shell
iproxy 2222:22
```

This would allow using ssh with `localhost:2222` to connect to the sshd daemon
on the device. Please mind that this is just an example and the sshd daemon is
only available for jailbroken devices that actually have it installed.

Please consult the usage information or manual page for a full documentation of
available command line options:
```shell
iproxy --help
man iproxy
```

### inetcat

This utility is a simple netcat-like tool that allows opening a read/write
interface to a TCP port on a usbmux device and expose it via STDIN/STDOUT.

Use ssh ProxyCommand to connect to a jailbroken iOS device via SSH:
```shell
ssh -oProxyCommand="inetcat 22" root@localhost
```

Please consult the usage information or manual page for a full documentation of
available command line options:
```shell
inetcat --help
man inetcat
```

### Environment

The environment variable `USBMUXD_SOCKET_ADDRESS` allows to change the location
of the usbmuxd socket away from the local default one.

An example of using an utility from the libimobiledevice project with an usbmuxd
socket exposed on a port of a remote host:
```shell
export USBMUXD_SOCKET_ADDRESS=192.168.179.1:27015
ideviceinfo
```

This sets the usbmuxd socket address to `192.168.179.1:27015` for applications
that use the libusbmuxd library.

## Contributing

We welcome contributions from anyone and are grateful for every pull request!

If you'd like to contribute, please fork the `master` branch, change, commit and
send a pull request for review. Once approved it can be merged into the main
code base.

If you plan to contribute larger changes or a major refactoring, please create a
ticket first to discuss the idea upfront to ensure less effort for everyone.

Please make sure your contribution adheres to:
* Try to follow the code style of the project
* Commit messages should describe the change well without being too short
* Try to split larger changes into individual commits of a common domain
* Use your real name and a valid email address for your commits

## Links

* Homepage: https://libimobiledevice.org/
* Repository: https://github.com/libimobiledevice/libusbmuxd.git
* Repository (Mirror): https://git.libimobiledevice.org/libusbmuxd.git
* Issue Tracker: https://github.com/libimobiledevice/libusbmuxd/issues
* Mailing List: https://lists.libimobiledevice.org/mailman/listinfo/libimobiledevice-devel
* Twitter: https://twitter.com/libimobiledev

## License

This library is licensed under the [GNU Lesser General Public License v2.1](https://www.gnu.org/licenses/lgpl-2.1.en.html),
also included in the repository in the `COPYING` file.

The utilities `iproxy` and `inetcat` are licensed under the [GNU General Public License v2.0](https://www.gnu.org/licenses/gpl-2.0.en.html).

## Credits

Apple, iPhone, iPad, iPod, iPod Touch, Apple TV, Apple Watch, Mac, iOS,
iPadOS, tvOS, watchOS, and macOS are trademarks of Apple Inc.

This project is an independent software library and has not been authorized,
sponsored, or otherwise approved by Apple Inc.

README Updated on: 2024-10-22
