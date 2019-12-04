/*
 * inetcat.c -- simple netcat-like tool that enables service access to iOS devices
 *
 * Copyright (C) 2017 Adrien Guinet <adrien@guinet.me>
 *
 * Based on iproxy which is based upon iTunnel source code, Copyright (c)
 * 2008 Jing Su.  http://www.cs.toronto.edu/~jingsu/itunnel/
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
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#ifdef WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <signal.h>
#endif

#include "usbmuxd.h"
#include "socket.h"

static size_t read_data_socket(int fd, uint8_t* buf, size_t bufsize)
{
#ifdef WIN32
    u_long bytesavailable = 0;
    if (fd == STDIN_FILENO) {
        bytesavailable = bufsize;
    } else if (ioctlsocket(fd, FIONREAD, &bytesavailable) != 0) {
        perror("ioctlsocket FIONREAD failed");
        exit(1);
    }
#else
    size_t bytesavailable = 0;
    if (ioctl(fd, FIONREAD, &bytesavailable) != 0) {
        perror("ioctl FIONREAD failed");
        exit(1);
    }
#endif
    size_t bufread = (bytesavailable >= bufsize) ? bufsize:bytesavailable;
    ssize_t ret = read(fd, buf, bufread);
    if (ret < 0) {
        perror("read failed");
        exit(1);
    }
    return (size_t)ret;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s DEVICE_TCP_PORT [UDID]\n", argv[0]);
        return 1;
    }

    int device_port = atoi(argv[1]);
    char* device_udid = NULL;

    if (argc > 2) {
        device_udid = argv[2];
    }

    if (!device_port) {
        fprintf(stderr, "Invalid device_port specified!\n");
        return -EINVAL;
    }

#ifndef WIN32
    signal(SIGPIPE, SIG_IGN);
#endif
    usbmuxd_device_info_t *dev_list = NULL;
    int count;
    if ((count = usbmuxd_get_device_list(&dev_list)) < 0) {
        printf("Connecting to usbmuxd failed, terminating.\n");
        free(dev_list);
        return 1;
    }

    if (dev_list == NULL || dev_list[0].handle == 0) {
        fprintf(stderr, "No connected device found, terminating.\n");
        free(dev_list);
        return 1;
    }

    usbmuxd_device_info_t *dev = NULL;
    if (device_udid) {
        int i;
        for (i = 0; i < count; i++) {
            if (strncmp(dev_list[i].udid, device_udid, sizeof(dev_list[0].udid)) == 0) {
                dev = &(dev_list[i]);
                break;
            }
        }
    } else {
        dev = &(dev_list[0]);
    }

    if (dev == NULL || dev->handle == 0) {
        fprintf(stderr, "No connected/matching device found, disconnecting client.\n");
        free(dev_list);
        return 1;
    }

    int devfd = usbmuxd_connect(dev->handle, device_port);
    free(dev_list);
    if (devfd < 0) {
        fprintf(stderr, "Error connecting to device!\n");
        return 1;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    FD_SET(devfd, &fds);

    int ret = 0;
    uint8_t buf[4096];
    while (1) {
        fd_set read_fds = fds;
        int ret_sel = select(devfd+1, &read_fds, NULL, NULL, NULL);
        if (ret_sel < 0) {
            perror("select");
            ret = 1;
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            size_t n = read_data_socket(STDIN_FILENO, buf, sizeof(buf));
            if (n == 0) {
                break;
            }
            write(devfd, buf, n);
        }

        if (FD_ISSET(devfd, &read_fds)) {
            size_t n = read_data_socket(devfd, buf, sizeof(buf));
            if (n == 0) {
                break;
            }
            write(STDOUT_FILENO, buf, n);
        }
    }

    socket_close(devfd);
    return ret;
}
