#include <stdio.h>
#include <usb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "niusb6501.h"

void counter_demo(struct usb_dev_handle *handle)
{
	niusb6501_write_counter(handle, 0);
	niusb6501_start_counter(handle);

	printf("\n");

	for(;;)
	{
		unsigned long value;
		int status;

		status = niusb6501_read_counter(handle, &value);
		if(status)
		{
			fprintf(stderr, "error reading counter: %s\n", strerror(-status));
			break;
		}

		printf("\e[1ACOUNTER: %08x\n", value);

		usleep(200*1000);
	}

	niusb6501_stop_counter(handle);
}

void char_to_bin(unsigned char value, char *buffer)
{
	int i;

	for(i = 0; i < 8; i++)
		buffer[7-i] = value & (1 << i) ? '1' : '0';
	buffer[8] = '\0';
}

void read_port_demo(struct usb_dev_handle *handle)
{
	niusb6501_set_io_mode(handle, 0x00, 0x00, 0x00);

	printf("P0      |P1      |P2\n");
	printf("\n");

	for(;;)
	{
		unsigned char p0, p1, p2;
		char s0[9], s1[9], s2[9];
		int status;

		status = niusb6501_read_port(handle, 0, &p0);
		if(status)
		{
			fprintf(stderr, "error read port 0: %s\n", strerror(-status));
			break;
		}
		status = niusb6501_read_port(handle, 1, &p1);
		if(status)
		{
			fprintf(stderr, "error read port 1: %s\n", strerror(-status));
			break;
		}
		status = niusb6501_read_port(handle, 2, &p2);
		if(status)
		{
			fprintf(stderr, "error read port 2: %s\n", strerror(-status));
			break;
		}

		char_to_bin(p0, s0);
		char_to_bin(p1, s1);
		char_to_bin(p2, s2);

		printf("\e[1A%s|%s|%s\n", s0, s1, s2);

		usleep(200*1000);
	}
}

void write_port_demo(struct usb_dev_handle *handle)
{
	unsigned long value = 0x000000ff;

	niusb6501_set_io_mode(handle, 0xff, 0xff, 0xff);

	printf("P0      |P1      |P2\n");
	printf("\n");

	for(;;)
	{
		unsigned char p0, p1, p2;
		char s0[9], s1[9], s2[9];
		int status;

		p0 = (value >> 16) & 0xff;
		p1 = (value >> 8) & 0xff;
		p2 = value & 0xff;

		status = niusb6501_write_port(handle, 0, p0);
		if(status)
		{
			fprintf(stderr, "error write port 0: %s\n", strerror(-status));
			break;
		}
		status = niusb6501_write_port(handle, 1, p1);
		if(status)
		{
			fprintf(stderr, "error write port 1: %s\n", strerror(-status));
			break;
		}
		status = niusb6501_write_port(handle, 2, p2);
		if(status)
		{
			fprintf(stderr, "error write port 2: %s\n", strerror(-status));
			break;
		}

		char_to_bin(p0, s0);
		char_to_bin(p1, s1);
		char_to_bin(p2, s2);

		printf("\e[1A%s|%s|%s\n", s0, s1, s2);

		usleep(200*1000);

		value <<= 1;
		if(value & 0x01000000)
			value |= 0x00000001;
		value &= 0x00ffffff;
	}
}

void print_usage(const char *progname)
{
	fprintf(stderr,	"Usage: %s [mode]\n"
			"-c\tcounter demo\n"
			"-r\tport read demo\n"
			"-w\tport write demo\n"
			"-h\tshow help\n",
	        progname);
}

int main(int argc, char **argv)
{
	struct usb_device *dev;
	struct usb_dev_handle *handle;
	char mode = 'h';

	if(argc == 2 && argv[1][0] == '-' && argv[1][1] && !argv[1][2])
		mode = argv[1][1];

	switch(mode)
	{
	case 'c':
	case 'r':
	case 'w':
		break;
	case 'h':
	default:
		print_usage(argv[0]);
		return 0;
	}

	if(niusb6501_list_devices(&dev, 1) != 1)
	{
		fprintf(stderr, "Device not found\n");
		return ENODEV;
	}

	handle = niusb6501_open_device(dev);
	if (handle == NULL)
	{
		fprintf(stderr, "Unable to open the USB device: %s\n", strerror(errno));
		return errno;
	}

	printf("NI USB-6501 demo - press CTRL-C to quit\n\n");

	switch(mode)
	{
	case 'c':
		counter_demo(handle);
		break;
	case 'r':
		read_port_demo(handle);
		break;
	case 'w':
		write_port_demo(handle);
		break;
	}

	niusb6501_close_device(handle);

	return 0;
}
