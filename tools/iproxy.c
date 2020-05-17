/*
 * iproxy.c -- proxy that enables tcp service access to iOS devices
 *
 * Copyright (C) 2009-2020 Nikias Bassen <nikias@gmx.li>
 * Copyright (C) 2014      Martin Szulecki <m.szulecki@libimobiledevice.org>
 * Copyright (C) 2009      Paul Sladen <libiphone@paul.sladen.org>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
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
#include <signal.h>
#endif
#include "socket.h"
#include "usbmuxd.h"

#ifndef ETIMEDOUT
#define ETIMEDOUT 138
#endif

static int debug_level = 0;
static uint16_t listen_port = 0;
static uint16_t device_port = 0;
static char* device_udid = NULL;
static enum usbmux_lookup_options lookup_opts = 0;

struct client_data {
	int fd;
	int sfd;
	volatile int stop_ctos;
	volatile int stop_stoc;
};

static void *run_stoc_loop(void *arg)
{
	struct client_data *cdata = (struct client_data*)arg;
	int recv_len;
	int sent;
	char buffer[131072];

	printf("%s: fd = %d\n", __func__, cdata->fd);

	while (!cdata->stop_stoc && cdata->fd > 0 && cdata->sfd > 0) {
		recv_len = socket_receive_timeout(cdata->sfd, buffer, sizeof(buffer), 0, 5000);
		if (recv_len <= 0) {
			if (recv_len == 0 || recv_len == -ETIMEDOUT) {
				// try again
				continue;
			} else {
				fprintf(stderr, "recv failed: %s\n", strerror(-recv_len));
				break;
			}
		} else {
			// send to socket
			sent = socket_send(cdata->fd, buffer, recv_len);
			if (sent < recv_len) {
				if (sent <= 0) {
					fprintf(stderr, "send failed: %s\n", strerror(errno));
					break;
				} else {
					fprintf(stderr, "only sent %d from %d bytes\n", sent, recv_len);
				}
			}
		}
	}

	socket_close(cdata->fd);

	cdata->fd = -1;
	cdata->stop_ctos = 1;

	return NULL;
}

static void *run_ctos_loop(void *arg)
{
	struct client_data *cdata = (struct client_data*)arg;
	int recv_len;
	int sent;
	char buffer[131072];
#ifdef WIN32
	HANDLE stoc = NULL;
#else
	pthread_t stoc;
#endif

	printf("%s: fd = %d\n", __func__, cdata->fd);

	cdata->stop_stoc = 0;
#ifdef WIN32
	stoc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)run_stoc_loop, cdata, 0, NULL);
#else
	pthread_create(&stoc, NULL, run_stoc_loop, cdata);
#endif

	while (!cdata->stop_ctos && cdata->fd>0 && cdata->sfd>0) {
		recv_len = socket_receive_timeout(cdata->fd, buffer, sizeof(buffer), 0, 5000);
		if (recv_len <= 0) {
			if (recv_len == 0 || recv_len == -ETIMEDOUT) {
				// try again
				continue;
			} else {
				fprintf(stderr, "recv failed: %s\n", strerror(-recv_len));
				break;
			}
		} else {
			// send to local socket
			sent = socket_send(cdata->sfd, buffer, recv_len);
			if (sent < recv_len) {
				if (sent <= 0) {
					fprintf(stderr, "send failed: %s\n", strerror(errno));
					break;
				} else {
					fprintf(stderr, "only sent %d from %d bytes\n", sent, recv_len);
				}
			}
		}
	}

	socket_close(cdata->fd);

	cdata->fd = -1;
	cdata->stop_stoc = 1;

#ifdef WIN32
	WaitForSingleObject(stoc, INFINITE);
#else
	pthread_join(stoc, NULL);
#endif

	return NULL;
}

static void *acceptor_thread(void *arg)
{
	struct client_data *cdata;
	usbmuxd_device_info_t *dev_list = NULL;
	usbmuxd_device_info_t *dev = NULL;
	usbmuxd_device_info_t muxdev;
#ifdef WIN32
	HANDLE ctos = NULL;
#else
	pthread_t ctos;
#endif
	int count;

	if (!arg) {
		fprintf(stderr, "invalid client_data provided!\n");
		return NULL;
	}

	cdata = (struct client_data*)arg;

	if (device_udid) {
		if (usbmuxd_get_device(device_udid, &muxdev, lookup_opts) > 0) {
			dev = &muxdev;
		}
	} else {
		if ((count = usbmuxd_get_device_list(&dev_list)) < 0) {
			printf("Connecting to usbmuxd failed, terminating.\n");
			free(dev_list);
			if (cdata->fd > 0) {
				socket_close(cdata->fd);
			}
			free(cdata);
			return NULL;
		}

		if (dev_list == NULL || dev_list[0].handle == 0) {
			printf("No connected device found, terminating.\n");
			free(dev_list);
			if (cdata->fd > 0) {
				socket_close(cdata->fd);
			}
			free(cdata);
			return NULL;
		}

		int i;
		for (i = 0; i < count; i++) {
			if (dev_list[i].conn_type == CONNECTION_TYPE_USB && (lookup_opts & DEVICE_LOOKUP_USBMUX)) {
				dev = &(dev_list[i]);
				break;
			} else if (dev_list[i].conn_type == CONNECTION_TYPE_NETWORK && (lookup_opts & DEVICE_LOOKUP_NETWORK)) {
				dev = &(dev_list[i]);
				break;
			}
		}
	}

	if (dev == NULL || dev->handle == 0) {
		printf("No connected/matching device found, disconnecting client.\n");
		free(dev_list);
		if (cdata->fd > 0) {
			socket_close(cdata->fd);
		}
		free(cdata);
		return NULL;
	}

	fprintf(stdout, "Requesting connecion to %s device handle == %d (serial: %s), port %d\n", (dev->conn_type == CONNECTION_TYPE_NETWORK) ? "NETWORK" : "USB", dev->handle, dev->udid, device_port);

	cdata->sfd = usbmuxd_connect(dev->handle, device_port);
	free(dev_list);
	if (cdata->sfd < 0) {
		fprintf(stderr, "Error connecting to device!\n");
	} else {
		cdata->stop_ctos = 0;

#ifdef WIN32
		ctos = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)run_ctos_loop, cdata, 0, NULL);
		WaitForSingleObject(ctos, INFINITE);
#else
		pthread_create(&ctos, NULL, run_ctos_loop, cdata);
		pthread_join(ctos, NULL);
#endif
	}

	if (cdata->fd > 0) {
		socket_close(cdata->fd);
	}
	if (cdata->sfd > 0) {
		socket_close(cdata->sfd);
	}
	free(cdata);

	return NULL;
}

static void print_usage(int argc, char **argv, int is_error)
{
	char *name = NULL;
	name = strrchr(argv[0], '/');
	fprintf(is_error ? stderr : stdout, "Usage: %s [OPTIONS] LOCAL_PORT DEVICE_PORT\n", (name ? name + 1: argv[0]));
	fprintf(is_error ? stderr : stdout,
	  "Proxy that enables TCP service access to iOS devices.\n\n" \
	  "  -u, --udid UDID    target specific device by UDID\n" \
	  "  -n, --network      connect to network device\n" \
	  "  -l, --local        connect to USB device (default)\n" \
	  "  -h, --help         prints usage information\n" \
	  "  -d, --debug        increase debug level\n" \
	  "\n" \
	  "Homepage: <" PACKAGE_URL ">\n"
	  "Bug reports: <" PACKAGE_BUGREPORT ">\n"
	  "\n"
	);
}

int main(int argc, char **argv)
{
	int mysock = -1;
	const struct option longopts[] = {
		{ "debug", no_argument, NULL, 'd' },
		{ "help", no_argument, NULL, 'h' },
		{ "udid", required_argument, NULL, 'u' },
		{ "local", no_argument, NULL, 'l' },
		{ "network", no_argument, NULL, 'n' },
		{ NULL, 0, NULL, 0}
	};
	int c = 0;
	while ((c = getopt_long(argc, argv, "dhu:ln", longopts, NULL)) != -1) {
		switch (c) {
		case 'd':
			libusbmuxd_set_debug_level(++debug_level);
			break;
		case 'u':
			if (!*optarg) {
				fprintf(stderr, "ERROR: UDID must not be empty!\n");
				print_usage(argc, argv, 1);
				return 2;
			}
			free(device_udid);
			device_udid = strdup(optarg);
			break;
		case 'l':
			lookup_opts |= DEVICE_LOOKUP_USBMUX;
			break;
		case 'n':
			lookup_opts |= DEVICE_LOOKUP_NETWORK;
			break;
		case 'h':
			print_usage(argc, argv, 0);
			return 0;
		default:
			print_usage(argc, argv, 1);
			return 2;
		}
	}

	if (lookup_opts == 0) {
		lookup_opts = DEVICE_LOOKUP_USBMUX;
	}

	argc -= optind;
	argv += optind;

	if (argc < 2) {
		print_usage(argc + optind, argv - optind, 1);
		free(device_udid);
		return 2;
	}

	listen_port = atoi(argv[0]);
	device_port = atoi(argv[1]);

	if (!listen_port) {
		fprintf(stderr, "Invalid listen port specified!\n");
		free(device_udid);
		return -EINVAL;
	}

	if (!device_port) {
		fprintf(stderr, "Invalid device port specified!\n");
		free(device_udid);
		return -EINVAL;
	}

#ifndef WIN32
	signal(SIGPIPE, SIG_IGN);
#endif
	// first create the listening socket endpoint waiting for connections.
	mysock = socket_create(listen_port);
	if (mysock < 0) {
		fprintf(stderr, "Error creating socket: %s\n", strerror(errno));
		free(device_udid);
		return -errno;
	} else {
#ifdef WIN32
		HANDLE acceptor = NULL;
#else
		pthread_t acceptor;
#endif
		struct client_data *cdata;
		int c_sock;
		while (1) {
			printf("waiting for connection\n");
			c_sock = socket_accept(mysock, listen_port);
			if (c_sock) {
				printf("accepted connection, fd = %d\n", c_sock);
				cdata = (struct client_data*)malloc(sizeof(struct client_data));
				if (!cdata) {
					socket_close(c_sock);
					fprintf(stderr, "ERROR: Out of memory\n");
					free(device_udid);
					return -1;
				}
				cdata->fd = c_sock;
#ifdef WIN32
				acceptor = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)acceptor_thread, cdata, 0, NULL);
				CloseHandle(acceptor);
#else
				pthread_create(&acceptor, NULL, acceptor_thread, cdata);
				pthread_detach(acceptor);
#endif
			} else {
				break;
			}
		}
		socket_close(c_sock);
		socket_close(mysock);
	}

	free(device_udid);

	return 0;
}
