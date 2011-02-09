#include <stdio.h>
#include <usb.h>
#include <string.h>
#include <errno.h>

#define VENDOR_ID	0x3923
#define PRODUCT_ID	0x718a

#define EP_IN	0x81
#define EP_OUT	0x01

#define TIMEOUT 1000

#define PROTOCOL_ERROR		(-EPROTO)
#define BUFFER_TOO_SMALL	(-ENOSPC)

#if 0
#define DEBUG(args...) fprintf(stderr , ## args);
#else
#define DEBUG(args...)
#endif

static struct usb_device *device_init(void)
{
    struct usb_bus *usb_bus;
    struct usb_device *dev;

    usb_init();
    usb_find_busses();
    usb_find_devices();

    for (usb_bus = usb_busses;
         usb_bus;
         usb_bus = usb_bus->next) {
        for (dev = usb_bus->devices;
             dev;
             dev = dev->next) {
            if ((dev->descriptor.idVendor
                  == VENDOR_ID) &&
                (dev->descriptor.idProduct
                  == PRODUCT_ID))
                return dev;
        }
    }
    return NULL;
}

int send_request(usb_dev_handle *usb_handle, size_t request_len, void *request, size_t *result_len, void *result)
{
	unsigned char buffer[128];
	int status;

	if(request_len > 255-4)
	{
		fprintf(stderr, "message too large (%d > 255)\n", request_len);
		exit(1);
	}

	buffer[0] = 0x00;
	buffer[1] = 0x01;
	buffer[2] = 0x00;
	buffer[3] = request_len+4;

	memcpy(&buffer[4], request, request_len);

	/* send command */
	DEBUG("send command\n");
	status = usb_bulk_write(
		usb_handle,
		EP_OUT,
		buffer,
		request_len+4,
		TIMEOUT
	);
	if(status < 0)
		return status;

	/* read result */
	DEBUG("read result\n");
	status = usb_bulk_read(
		usb_handle,
		EP_IN,
		buffer,
		sizeof buffer,
		TIMEOUT
	);
	if(status < 0)
		return status;
	if(status < 4)
		return PROTOCOL_ERROR;
	if(status-4 > *result_len)
		return BUFFER_TOO_SMALL;
	*result_len = status-4;
	memcpy(result, &buffer[4], status-4);

	return 0;
}

void dump_buffer(size_t len, void *buffer)
{
	size_t i;

	printf("%02x: ", len);
	for(i = 0; i < len; i++)
		printf("%02x ", ((unsigned char *) buffer)[i]);
	printf("\n");
}

void dump_request(int status, size_t request_len, void *request, size_t result_len, void *result)
{
	printf("-> ");
	dump_buffer(request_len, request);
	printf("<- ");
	if(status < 0)
		printf("%s\n", strerror(-status));
	else
		dump_buffer(result_len, result);
}

int main(int argc, char **argv)
{
	struct usb_device *usb_dev;
	struct usb_dev_handle *usb_handle;

	usb_dev = device_init();
	if (usb_dev == NULL)
	{
		fprintf(stderr, "Device not found\n");
		return -ENODEV;
	}

	usb_handle = usb_open(usb_dev);
	if (usb_handle == NULL)
	{
		fprintf(stderr, "Unable to open the USB device: %s\n", strerror(errno));
		return errno;
	}

	char request[64];
	char result[64];
	int status;
	size_t request_len, result_len;

#if 1
	// set I/O mask
	result_len = sizeof result;
	request_len = 20;
	memcpy(request, "\x00\x14\x01\x12\x02\x10\x00\x00\x00\x05\xff\x00\x00\x00\x05\x00\x00\x00\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);

	// read port 0
	result_len = sizeof result;
	request_len = 12;
	memcpy(request, "\x00\x0C\x01\x0E\x02\x10\x00\x00\x00\x03\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);

	// write port 0
	result_len = sizeof result;
	request_len = 16;
	memcpy(request, "\x00\x10\x01\x0F\x02\x10\x00\x00\x00\x03\x00\x00\x03\xff\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);

	// read port 0
	result_len = sizeof result;
	request_len = 12;
	memcpy(request, "\x00\x0C\x01\x0E\x02\x10\x00\x00\x00\x03\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);
#endif

#if 0
	// reset counter
	result_len = sizeof result;
	request_len = 12;
	memcpy(request, "\x00\x0C\x01\x0F\x02\x20\x00\x00\x00\x00\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);

	// start counter
	result_len = sizeof result;
	request_len = 8;
	memcpy(request, "\x00\x08\x01\x09\x02\x20\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);

	// stop counter
	result_len = sizeof result;
	request_len = 8;
	memcpy(request, "\x00\x08\x01\x0C\x02\x20\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);

	// read counter
	result_len = sizeof result;
	request_len = 8;
	memcpy(request, "\x00\x08\x01\x0E\x02\x20\x00\x00", request_len);
	status = send_request(usb_handle, request_len, request, &result_len, result);
	dump_request(status, request_len, request, result_len, result);
#endif

	return 0;
}
