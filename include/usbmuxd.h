/*
 * usbmuxd.h - A client library to talk to the usbmuxd daemon.
 *
 * Copyright (C) 2014 Martin Szulecki <m.szulecki@libimobiledevice.org>
 * Copyright (C) 2009 Nikias Bassen <nikias@gmx.li>
 * Copyright (C) 2009 Paul Sladen <libiphone@paul.sladen.org>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef USBMUXD_H
#define USBMUXD_H
#include <stdint.h>

#ifdef WIN32
#define USBMUXD_API __declspec( dllexport )
#else
#ifdef HAVE_FVISIBILITY
#define USBMUXD_API __attribute__((visibility("default")))
#else
#define USBMUXD_API
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Device information structure holding data to identify the device.
 * The relevant 'handle' should be passed to 'usbmuxd_connect()', to
 * start a proxy connection.  The value 'handle' should be considered
 * opaque and no presumption made about the meaning of its value.
 */
typedef struct {
	uint32_t handle;
	int product_id;
	char udid[41];
} usbmuxd_device_info_t;

/**
 * event types for event callback function
 */
enum usbmuxd_event_type {
    UE_DEVICE_ADD = 1,
    UE_DEVICE_REMOVE
};

/**
 * Event structure that will be passed to the callback function.
 * 'event' will contains the type of the event, and 'device' will contains
 * information about the device.
 */
typedef struct {
    int event;
    usbmuxd_device_info_t device;
} usbmuxd_event_t;

/**
 * Callback function prototype.
 */
typedef void (*usbmuxd_event_cb_t) (const usbmuxd_event_t *event, void *user_data);

/**
 * Subscribe a callback function so that applications get to know about
 * device add/remove events.
 *
 * @param callback A callback function that is executed when an event occurs.
 *
 * @return 0 on success or negative on error.
 */
USBMUXD_API int usbmuxd_subscribe(usbmuxd_event_cb_t callback, void *user_data);

/**
 * Unsubscribe callback.
 *
 * @return only 0 for now.
 */
USBMUXD_API int usbmuxd_unsubscribe();

/**
 * Contacts usbmuxd and retrieves a list of connected devices.
 *
 * @param device_list A pointer to an array of usbmuxd_device_info_t
 *      that will hold records of the connected devices. The last record
 *      is a null-terminated record with all fields set to 0/NULL.
 * @note The user has to free the list returned.
 *
 * @return number of attached devices, zero on no devices, or negative
 *   if an error occured.
 */
USBMUXD_API int usbmuxd_get_device_list(usbmuxd_device_info_t **device_list);

/**
 * Frees the device list returned by an usbmuxd_get_device_list call
 *
 * @param device_list A pointer to an array of usbmuxd_device_info_t to free.
 *
 * @return 0 on success, -1 on error.
 */
USBMUXD_API int usbmuxd_device_list_free(usbmuxd_device_info_t **device_list);

/**
 * Gets device information for the device specified by udid.
 *
 * @param udid A device UDID of the device to look for. If udid is NULL,
 *      This function will return the first device found.
 * @param device Pointer to a previously allocated (or static) 
 *      usbmuxd_device_info_t that will be filled with the device info.
 *
 * @return 0 if no matching device is connected, 1 if the device was found,
 *    or a negative value on error.
 */
USBMUXD_API int usbmuxd_get_device_by_udid(const char *udid, usbmuxd_device_info_t *device);

/**
 * Request proxy connect to 
 *
 * @param handle returned by 'usbmuxd_scan()'
 *
 * @param tcp_port TCP port number on device, in range 0-65535.
 *	common values are 62078 for lockdown, and 22 for SSH.
 *
 * @return file descriptor socket of the connection, or -1 on error
 */
USBMUXD_API int usbmuxd_connect(const int handle, const unsigned short tcp_port);

/**
 * Disconnect. For now, this just closes the socket file descriptor.
 *
 * @param sfd socker file descriptor returned by usbmuxd_connect()
 *
 * @return 0 on success, -1 on error.
 */
USBMUXD_API int usbmuxd_disconnect(int sfd);

/**
 * Send data to the specified socket.
 *
 * @param sfd socket file descriptor returned by usbmuxd_connect()
 * @param data buffer to send
 * @param len size of buffer to send
 * @param sent_bytes how many bytes sent
 *
 * @return 0 on success, a negative errno value otherwise.
 */
USBMUXD_API int usbmuxd_send(int sfd, const char *data, uint32_t len, uint32_t *sent_bytes);

/**
 * Receive data from the specified socket.
 *
 * @param sfd socket file descriptor returned by usbmuxd_connect()
 * @param data buffer to put the data to
 * @param len number of bytes to receive
 * @param recv_bytes number of bytes received
 * @param timeout how many milliseconds to wait for data
 *
 * @return 0 on success, a negative errno value otherwise.
 */
USBMUXD_API int usbmuxd_recv_timeout(int sfd, char *data, uint32_t len, uint32_t *recv_bytes, unsigned int timeout);

/**
 * Receive data from the specified socket with a default timeout.
 *
 * @param sfd socket file descriptor returned by usbmuxd_connect()
 * @param data buffer to put the data to
 * @param len number of bytes to receive
 * @param recv_bytes number of bytes received
 *
 * @return 0 on success, a negative errno value otherwise.
 */
USBMUXD_API int usbmuxd_recv(int sfd, char *data, uint32_t len, uint32_t *recv_bytes);

/**
 * Reads the SystemBUID
 *
 * @param buid pointer to a variable that will be set to point to a newly
 *     allocated string with the System BUID returned by usbmuxd
 *
 * @return 0 on success, a negative errno value otherwise.
 */
USBMUXD_API int usbmuxd_read_buid(char** buid);

/**
 * Read a pairing record
 *
 * @param record_id the record identifier of the pairing record to retrieve
 * @param record_data pointer to a variable that will be set to point to a
 *     newly allocated buffer containing the pairing record data
 * @param record_size pointer to a variable that will be set to the size of
 *     the buffer returned in record_data
 *
 * @return 0 on success, a negative error value otherwise.
 */
USBMUXD_API int usbmuxd_read_pair_record(const char* record_id, char **record_data, uint32_t *record_size);

/**
 * Save a pairing record
 *
 * @param record_id the record identifier of the pairing record to save
 * @param record_data buffer containing the pairing record data
 * @param record_size size of the buffer passed in record_data
 *
 * @return 0 on success, a negative error value otherwise.
 */
USBMUXD_API int usbmuxd_save_pair_record(const char* record_id, const char *record_data, uint32_t record_size);

/**
 * Delete a pairing record
 *
 * @param record_id the record identifier of the pairing record to delete.
 *
 * @return 0 on success, a negative errno value otherwise.
 */
USBMUXD_API int usbmuxd_delete_pair_record(const char* record_id);

/**
 * Enable or disable the use of inotify extension. Enabled by default.
 * Use 0 to disable and 1 to enable inotify support.
 * This only has an effect on linux systems if inotify support has been built
 * in. Otherwise and on all other platforms this function has no effect.
 */
USBMUXD_API void libusbmuxd_set_use_inotify(int set);

USBMUXD_API void libusbmuxd_set_debug_level(int level);

#ifdef __cplusplus
}
#endif

#endif /* USBMUXD_H */
