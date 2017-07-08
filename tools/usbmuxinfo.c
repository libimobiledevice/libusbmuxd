/*
 * iproxy.c -- proxy that enables tcp service access to iOS devices
 *
 * Copyright (C) 2014	Martin Szulecki <m.szulecki@libimobiledevice.org>
 * Copyright (C) 2009	Nikias Bassen <nikias@gmx.li>
 * Copyright (C) 2009	Paul Sladen <libiphone@paul.sladen.org>
 *
 * Based upon iTunnel source code, Copyright (c) 2008 Jing Su.
 * http://www.cs.toronto.edu/~jingsu/itunnel/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
typedef unsigned int socklen_t;
#else
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netinet/in.h>
#endif
#include "socket.h"
#include "usbmuxd.h"



int main(int argc, char **argv)
{
  usbmuxd_device_info_t* devices = NULL;
  int count = usbmuxd_get_device_list(&devices);

  for (int i = 0; i < count; i++) {
    printf("Handle = %u\n", devices[i].handle);
    printf("Product Id = %i\n", devices[i].product_id);
    printf("UDID = %.*s\n", 41, devices[i].udid);
  }

  usbmuxd_device_list_free(&devices);
}
