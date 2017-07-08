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

void scan_for_device(usbmuxd_device_info_t* device);

int main(int argc, char **argv)
{
  //libusbmuxd_set_debug_level(1);

  if (argc == 0) {
    usbmuxd_device_info_t* devices = NULL;

    int count = usbmuxd_get_device_list(&devices);

    for (int dev = 0; dev < count; dev++) {
      scan_for_device(&devices[dev]);
    }

    usbmuxd_device_list_free(&devices);
  }
  else {
    usbmuxd_device_info_t device;
    const char* udid_specifier = argv[1];

    usbmuxd_get_device_by_udid(udid_specifier, &device);

    scan_for_device(&device);
  }
}


void scan_for_device(usbmuxd_device_info_t* device)
{
  printf("Handle = %u\n", device->handle);
  printf("Product Id = %i\n", device->product_id);
  printf("UDID = %.*s\n", 41, device->udid);

  for (unsigned short i = 0; i < 65535; i++) {
    int result = usbmuxd_connect(device->handle, i);

    if (result != -1) {
      printf("Found open port at %hu\n", i);
      usbmuxd_disconnect(result);
    }
    else {
      printf(".");
    }
  }
}
