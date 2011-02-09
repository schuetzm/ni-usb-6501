#ifndef _NIUSB6501_H_
#define _NIUSB6501_H_

#include <usb.h>

extern size_t niusb6501_list_devices(struct usb_device *devices[], size_t size);
extern struct usb_dev_handle *niusb6501_open_device(struct usb_device *device);
extern int niusb6501_close_device(struct usb_dev_handle *handle);

extern int niusb6501_start_counter(struct usb_dev_handle *handle);
extern int niusb6501_stop_counter(struct usb_dev_handle *handle);
extern int niusb6501_read_counter(struct usb_dev_handle *handle, unsigned long *value);
extern int niusb6501_write_counter(struct usb_dev_handle *handle, unsigned long value);

extern int niusb6501_read_port(struct usb_dev_handle *handle, unsigned char port, unsigned char *value);
extern int niusb6501_write_port(struct usb_dev_handle *handle, unsigned char port, unsigned char value);

extern int niusb6501_set_io_mode(struct usb_dev_handle *handle, unsigned char port0, unsigned char port1, unsigned char port2);

#endif

