/*
 * Written by Oron Peled <oron@actcom.co.il>
 * Copyright (C) 2008, Xorcom
 *
 * All rights reserved.
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#define	_GNU_SOURCE	/* for memrchr() */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <debug.h>
#include <xusb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define	DBG_MASK	0x01
#define	TIMEOUT	500
#define	MAX_RETRIES	10

struct xusb {
	struct usb_device	*dev;
	usb_dev_handle		*handle;
	const struct xusb_spec	*spec;
	char			iManufacturer[BUFSIZ];
	char			iProduct[BUFSIZ];
	char			iSerialNumber[BUFSIZ];
	char			iInterface[BUFSIZ];
	char			devpath_tail[PATH_MAX + 1];
	int			bus_num;
	int			device_num;
	int			interface_num;
	int			ep_out;
	int			ep_in;
	int			is_usb2;
	int			is_claimed;
	int			is_open;
	size_t			packet_size;
};

static void xusb_init();

/*
 * XTALK_OPTIONS:
 *     A white-space separated list of options, read from the environment
 *     variable of that name. Existing options:
 *
 *     - "use-clear-halt" -- force USB "clear_halt" operation during
 *                           device initialization (this is the default)
 *     - "no-use-clear-halt" -- force no USB "clear_halt" operation during
 *                           device initialization
 *     - "no-lock" -- prevent using global sempahore to serialize libusb
 *                    initialization. Previously done via "XUSB_NOLOCK"
 *                    environment variable.
 */
int xtalk_parse_options(void);
int xtalk_option_use_clear_halt(void);
int xtalk_option_no_lock(void);

void xusb_init_spec(struct xusb_spec *spec, char *name,
		uint16_t vendor_id, uint16_t product_id,
		int nifaces, int iface, int nep, int ep_out, int ep_in)
{
	DBG("Initialize %s: interfaces=%d using interface num=%d endpoints=%d "
		"(OUT=0x%02X, IN=0x%02X)\n",
		name, nifaces, iface, nep, ep_out, ep_in);
	memset(spec, 0, sizeof(*spec));
	spec->name = name;
	spec->num_interfaces = nifaces;
	spec->my_interface_num = iface;
	spec->num_endpoints = nep;
	spec->my_vendor_id = vendor_id;
	spec->my_product_id = product_id;
	spec->my_ep_in = ep_in;
	spec->my_ep_out = ep_out;
}

#define	EP_OUT(xusb)	((xusb)->spec->my_ep_out)
#define	EP_IN(xusb)	((xusb)->spec->my_ep_in)

/*
 * USB handling
 */

static int get_usb_string(struct xusb *xusb, uint8_t item, char *buf)
{
	char	tmp[BUFSIZ];
	int	ret;

	assert(xusb->handle);
	if (!item)
		return 0;
	ret = usb_get_string_simple(xusb->handle, item, tmp, BUFSIZ);
	if (ret <= 0)
		return ret;
	return snprintf(buf, BUFSIZ, "%s", tmp);
}

static const struct usb_interface_descriptor *get_interface(
		const struct usb_device *dev,
		int my_interface_num,
		int num_interfaces)
{
	const struct usb_interface		*interface;
	const struct usb_interface_descriptor	*iface_desc;
	const struct usb_config_descriptor	*config_desc;
	int					num_altsetting;

	config_desc = dev->config;
	if (!config_desc) {
		ERR("No configuration descriptor: strange USB1 controller?\n");
		return NULL;
	}
	if (num_interfaces && config_desc->bNumInterfaces != num_interfaces) {
		DBG("Wrong number of interfaces: have %d need %d\n",
			config_desc->bNumInterfaces, num_interfaces);
		return NULL;
	}
	interface = &config_desc->interface[my_interface_num];
	assert(interface != NULL);
	iface_desc = interface->altsetting;
	num_altsetting = interface->num_altsetting;
	assert(num_altsetting != 0);
	assert(iface_desc != NULL);
	return iface_desc;
}

static int match_interface(const struct usb_device *dev,
		const struct xusb_spec *spec)
{
	const struct usb_device_descriptor	*dev_desc;
	const struct usb_interface_descriptor	*iface_desc;

	dev_desc = &dev->descriptor;
	assert(dev_desc);
	DBG("Checking: %04X:%04X interfaces=%d interface num=%d endpoints=%d: "
			"\"%s\"\n",
			spec->my_vendor_id,
			spec->my_product_id,
			spec->num_interfaces,
			spec->my_interface_num,
			spec->num_endpoints,
			spec->name);
	if (dev_desc->idVendor != spec->my_vendor_id) {
		DBG("Wrong vendor id 0x%X\n", dev_desc->idVendor);
		return 0;
	}
	if (dev_desc->idProduct != spec->my_product_id) {
		DBG("Wrong product id 0x%X\n", dev_desc->idProduct);
		return 0;
	}
	iface_desc = get_interface(dev, spec->my_interface_num,
				spec->num_interfaces);
	if (!iface_desc) {
		ERR("Could not get interface descriptor of device: %s\n",
			usb_strerror());
		return 0;
	}
	if (iface_desc->bInterfaceClass != 0xFF) {
		DBG("Wrong interface class 0x%X\n",
			iface_desc->bInterfaceClass);
		return 0;
	}
	if (iface_desc->bInterfaceNumber != spec->my_interface_num) {
		DBG("Wrong interface number %d (expected %d)\n",
			iface_desc->bInterfaceNumber, spec->my_interface_num);
		return 0;
	}
	if (iface_desc->bNumEndpoints != spec->num_endpoints) {
		DBG("Wrong number of endpoints %d\n",
			iface_desc->bNumEndpoints);
		return 0;
	}
	return	1;
}

#define	GET_USB_STRING(xusb, from, item) \
		get_usb_string((xusb), (from)->item, xusb->item)

static int xusb_fill_strings(struct xusb *xusb)
{
	const struct usb_device_descriptor	*dev_desc;
	const struct usb_interface_descriptor	*iface_desc;


	dev_desc = &xusb->dev->descriptor;
	assert(dev_desc);
	if (GET_USB_STRING(xusb, dev_desc, iManufacturer) < 0) {
		ERR("Failed reading iManufacturer string: %s\n",
			usb_strerror());
		return 0;
	}
	if (GET_USB_STRING(xusb, dev_desc, iProduct) < 0) {
		ERR("Failed reading iProduct string: %s\n",
			usb_strerror());
		return 0;
	}
	if (GET_USB_STRING(xusb, dev_desc, iSerialNumber) < 0) {
		ERR("Failed reading iSerialNumber string: %s\n",
			usb_strerror());
		return 0;
	}
	iface_desc = get_interface(xusb->dev, xusb->interface_num, 0);
	if (!iface_desc) {
		ERR("Could not get interface descriptor of device: %s\n",
			usb_strerror());
		return 0;
	}
	if (GET_USB_STRING(xusb, iface_desc, iInterface) < 0) {
		ERR("Failed reading iInterface string: %s\n", usb_strerror());
		return 0;
	}
	return 1;
}

static int xusb_open(struct xusb *xusb)
{
	assert(xusb);
	if (xusb->is_open)
		return 1;
	xusb->handle = usb_open(xusb->dev);
	if (!xusb->handle) {
		ERR("Failed to open usb device '%s': %s\n",
			xusb->devpath_tail, usb_strerror());
		return 0;
	}
	xusb->is_open = 1;
	return 1;
}

int xusb_claim_interface(struct xusb *xusb)
{
	const struct usb_device_descriptor	*dev_desc;
	int					ret;

	assert(xusb);
	xusb_open(xusb);	/* If it's not open yet... */
	if (usb_claim_interface(xusb->handle, xusb->interface_num) != 0) {
		ERR("usb_claim_interface %d in '%s': %s\n",
			xusb->interface_num,
			xusb->devpath_tail,
			usb_strerror());
		return 0;
	}
	xusb->is_claimed = 1;
	xusb_fill_strings(xusb);
	dev_desc = &xusb->dev->descriptor;
	DBG("ID=%04X:%04X Manufacturer=[%s] Product=[%s] "
		"SerialNumber=[%s] Interface=[%s]\n",
		dev_desc->idVendor,
		dev_desc->idProduct,
		xusb->iManufacturer,
		xusb->iProduct,
		xusb->iSerialNumber,
		xusb->iInterface);
	if (xtalk_option_use_clear_halt()) {
		DBG("Using clear_halt()\n");
		if (usb_clear_halt(xusb->handle, EP_OUT(xusb)) != 0) {
			ERR("Clearing output endpoint: %s\n", usb_strerror());
			return 0;
		}
		if (usb_clear_halt(xusb->handle, EP_IN(xusb)) != 0) {
			ERR("Clearing input endpoint: %s\n", usb_strerror());
			return 0;
		}
	}
	ret = xusb_flushread(xusb);
	if (ret < 0) {
		ERR("xusb_flushread failed: %d\n", ret);
		return 0;
	}
	return 1;
}

static void xusb_list_dump(struct xlist_node *xusb_list)
{
	struct xlist_node	*curr;
	struct xusb		*xusb;

	for (curr = xusb_list->next; curr != xusb_list; curr = curr->next) {
		struct usb_device		*dev;
		struct usb_bus			*bus;
		struct usb_device_descriptor	*dev_desc;

		xusb = curr->data;
		assert(xusb);
		dev = xusb->dev;
		assert(dev);
		bus = dev->bus;
		assert(bus);
		dev_desc = &dev->descriptor;
		assert(dev_desc);
		DBG("usb:ID=%04X:%04X [%s / %s / %s], (%s/%s)\n",
			dev_desc->idVendor,
			dev_desc->idProduct,
			xusb->iManufacturer,
			xusb->iProduct,
			xusb->iSerialNumber,
			bus->dirname,
			dev->filename
			);
	}
}

void xusb_destroy(struct xusb *xusb)
{
	if (xusb) {
		xusb_close(xusb);
		memset(xusb, 0, sizeof(*xusb));
		free(xusb);
	}
}

static struct xusb *xusb_new(struct usb_device *dev,
		const struct xusb_spec *spec)
{
	struct usb_device_descriptor	*dev_desc;
	struct usb_config_descriptor	*config_desc;
	struct usb_interface		*interface;
	struct usb_interface_descriptor	*iface_desc;
	struct usb_endpoint_descriptor	*endpoint;
	size_t				max_packet_size;
	int				i;
	struct xusb			*xusb = NULL;

	/*
	 * Get information from the usb_device
	 */
	dev_desc = &dev->descriptor;
	if (!dev_desc) {
		ERR("usb device without a device descriptor\n");
		goto fail;
	}
	config_desc = dev->config;
	if (!config_desc) {
		ERR("usb device without a configuration descriptor\n");
		goto fail;
	}
	interface = &config_desc->interface[spec->my_interface_num];
	iface_desc = interface->altsetting;
	endpoint = iface_desc->endpoint;
	/* Calculate max packet size */
	max_packet_size = PACKET_SIZE;
	for (i = 0; i < iface_desc->bNumEndpoints; i++, endpoint++) {
		DBG("Validating endpoint @ %d (interface %d)\n",
			i, spec->my_interface_num);
		if (endpoint->bEndpointAddress == spec->my_ep_out ||
			endpoint->bEndpointAddress == spec->my_ep_in) {
			if (endpoint->wMaxPacketSize > PACKET_SIZE) {
				ERR("EP #%d wMaxPacketSize too large (%d)\n",
					i, endpoint->wMaxPacketSize);
				goto fail;
			}
			if (endpoint->wMaxPacketSize < max_packet_size)
				max_packet_size = endpoint->wMaxPacketSize;
		}
	}
	/* Fill xusb */
	xusb = malloc(sizeof(*xusb));
	if (!xusb) {
		ERR("Out of memory");
		goto fail;
	}
	memset(xusb, 0, sizeof(*xusb));
	xusb->dev = dev;
	xusb->spec = spec;
	sscanf(dev->bus->dirname, "%d", &xusb->bus_num);
	sscanf(dev->filename, "%d", &xusb->device_num);
	snprintf(xusb->devpath_tail, PATH_MAX, "%03d/%03d",
		xusb->bus_num, xusb->device_num);
	xusb->interface_num = spec->my_interface_num;
	xusb->ep_out = spec->my_ep_out;
	xusb->ep_in = spec->my_ep_in;
	xusb->packet_size = max_packet_size;
	xusb->is_usb2 = (max_packet_size == 512);
	if (!xusb_open(xusb)) {
		ERR("Failed opening device: %04X:%04X - %s\n",
			dev_desc->idVendor,
			dev_desc->idProduct,
			xusb->devpath_tail);
		goto fail;
	}
	DBG("%04X:%04X - %s\n",
		dev_desc->idVendor,
		dev_desc->idProduct,
		xusb->devpath_tail);
	return xusb;
fail:
	xusb_destroy(xusb);
	return NULL;
}

struct xusb *xusb_find_iface(const char *devpath,
	int iface_num,
	int ep_out,
	int ep_in,
	struct xusb_spec *dummy_spec)
{
	struct usb_bus		*bus;

	DBG("\n");
	xusb_init();
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		int			bus_num;
		char			tmppath[PATH_MAX + 1];
		struct usb_device	*dev;

		tmppath[0] = '\0';
		sscanf(bus->dirname, "%d", &bus_num);
		snprintf(tmppath, sizeof(tmppath), "%03d", bus_num);
		DBG("Check bus %d: %s ? %s\n", bus_num, tmppath, devpath);
		if (strncmp(tmppath, devpath, strlen(tmppath)) != 0)
			continue;
		DBG("Matched bus %d\n", bus_num);
		for (dev = bus->devices; dev; dev = dev->next) {
			struct usb_device_descriptor	*dev_desc;
			struct usb_config_descriptor	*config_desc;
			struct usb_interface		*interface;
			struct xusb			*xusb;
			int				device_num;

			sscanf(dev->filename, "%d", &device_num);
			DBG("Check device %d\n", device_num);
			snprintf(tmppath, sizeof(tmppath), "%03d/%03d",
				bus_num, device_num);
			if (strncmp(tmppath, devpath, strlen(tmppath)) != 0)
				continue;
			dev_desc = &dev->descriptor;
			assert(dev_desc);
			config_desc = dev->config;
			assert(config_desc);
			interface = config_desc->interface;
			assert(interface);
			DBG("Matched device %s: %X:%X\n", tmppath,
				dev_desc->idVendor, dev_desc->idProduct);
			assert(dummy_spec);
			xusb_init_spec(dummy_spec, "<none>",
				dev_desc->idVendor, dev_desc->idProduct,
				config_desc->bNumInterfaces,
				iface_num,
				interface->altsetting->bNumEndpoints,
				ep_out, ep_in);
			xusb = xusb_new(dev, dummy_spec);
			if (!xusb)
				ERR("xusb allocation failed\n");
			return xusb;
		}
	}
	return NULL;
}

static const char *path_tail(const char *path)
{
	const char	*p;

	assert(path != NULL);
	/* Find last '/' */
	p = memrchr(path, '/', strlen(path));
	if (!p) {
		ERR("Missing a '/' in %s\n", path);
		return NULL;
	}
	/* Search for a '/' before that */
	p = memrchr(path, '/', p - path);
	if (!p)
		p = path;		/* No more '/' */
	else
		p++;			/* skip '/' */
	return p;
}

int xusb_filter_bypath(const struct xusb *xusb, void *data)
{
	const char	*p;
	const char	*path = data;

	DBG("%s\n", path);
	assert(path != NULL);
	p = path_tail(path);
	if (strcmp(xusb->devpath_tail, p) != 0) {
		DBG("device path missmatch: '%s' != '%s'\n",
			xusb->devpath_tail, p);
		return 0;
	}
	return 1;
}

struct xusb *xusb_find_bypath(const struct xusb_spec *specs, int numspecs,
			const char *path)
{
	struct xlist_node	*xlist;
	struct xlist_node	*head;
	struct xusb		*xusb;

	xlist = xusb_find_byproduct(specs, numspecs,
			xusb_filter_bypath, (void *)path);
	head = xlist_shift(xlist);
	if (!head)
		return NULL;
	if (!xlist_empty(xlist)) {
		ERR("Too many matches (extra %zd) to '%s'\n",
			xlist_length(xlist), path);
		return NULL;
	}
	xusb = head->data;
	xlist_destroy(xlist, NULL);
	return xusb;
}

struct xlist_node *xusb_find_byproduct(const struct xusb_spec *specs,
		int numspecs, xusb_filter_t filterfunc, void *data)
{
	struct xlist_node	*xlist;
	struct usb_bus		*bus;
	struct usb_device	*dev;

	DBG("specs(%d)\n", numspecs);
	xlist = xlist_new(NULL);
	if (!xlist) {
		ERR("Failed allocation new xlist");
		goto fail_xlist;
	}
	xusb_init();
	for (bus = usb_get_busses(); bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			struct usb_device_descriptor	*dev_desc;
			struct xlist_node		*item;
			int				i;

			dev_desc = &dev->descriptor;
			assert(dev_desc);
			DBG("usb:%s/%s: ID=%04X:%04X\n",
				dev->bus->dirname,
				dev->filename,
				dev_desc->idVendor,
				dev_desc->idProduct);
			for (i = 0; i < numspecs; i++) {
				struct xusb		*xusb;
				const struct xusb_spec	*sp = &specs[i];

				if (!match_interface(dev, sp))
					continue;
				xusb = xusb_new(dev, sp);
				if (!xusb) {
					ERR("xusb allocation failed\n");
					goto fail_malloc;
				}
				if (filterfunc && !filterfunc(xusb, data)) {
					xusb_destroy(xusb);
					continue;
				}
				item = xlist_new(xusb);
				xlist_append_item(xlist, item);
				break;
			}
		}
	}
	xusb_list_dump(xlist);
	return xlist;
fail_malloc:
	xlist_destroy(xlist, NULL);
fail_xlist:
	return NULL;
}

struct xusb *xusb_open_one(const struct xusb_spec *specs, int numspecs,
		xusb_filter_t filterfunc, void *data)
{
	struct xlist_node	*xusb_list;
	struct xlist_node	*curr;
	int			num;
	struct xusb		*xusb = NULL;

	xusb_list = xusb_find_byproduct(specs, numspecs, filterfunc, data);
	num = xlist_length(xusb_list);
	DBG("total %d devices\n", num);
	switch (num) {
	case 0:
		ERR("No matching device.\n");
		break;
	case 1:
		curr = xlist_shift(xusb_list);
		xusb = curr->data;
		xlist_destroy(curr, NULL);
		xlist_destroy(xusb_list, NULL);
		if (!xusb_claim_interface(xusb)) {
			xusb_destroy(xusb);
			return NULL;
		}
		xusb_showinfo(xusb);
		break;
	default:
		ERR("Too many devices (%d). Aborting.\n", num);
		break;
	}
	return xusb;
}

int xusb_interface(struct xusb *xusb)
{
	return xusb->interface_num;
}

size_t xusb_packet_size(const struct xusb *xusb)
{
	return xusb->packet_size;
}

/*
 * MP device handling
 */
void xusb_showinfo(const struct xusb *xusb)
{
	struct usb_device_descriptor	*dev_desc;
	struct usb_device		*dev;

	assert(xusb != NULL);
	dev = xusb->dev;
	dev_desc = &dev->descriptor;
	if (verbose <= LOG_INFO) {
		INFO("usb:%s/%s: ID=%04X:%04X [%s / %s / %s]\n",
			dev->bus->dirname,
			dev->filename,
			dev_desc->idVendor,
			dev_desc->idProduct,
			xusb->iManufacturer,
			xusb->iProduct,
			xusb->iSerialNumber);
	} else {
		printf("USB    Bus/Device:    [%s/%s] (%s,%s)\n",
			dev->bus->dirname,
			dev->filename,
			(xusb->is_open) ? "open" : "closed",
			(xusb->is_claimed) ? "claimed" : "unused");
		printf("USB    Spec name:     [%s]\n", xusb->spec->name);
		printf("USB    iManufacturer: [%s]\n", xusb->iManufacturer);
		printf("USB    iProduct:      [%s]\n", xusb->iProduct);
		printf("USB    iSerialNumber: [%s]\n", xusb->iSerialNumber);
	}
}

const char *xusb_serial(const struct xusb *xusb)
{
	return xusb->iSerialNumber;
}

const char *xusb_devpath(const struct xusb *xusb)
{
	return xusb->devpath_tail;
}

const char *xusb_manufacturer(const struct xusb *xusb)
{
	return xusb->iManufacturer;
}

const char *xusb_product(const struct xusb *xusb)
{
	return xusb->iProduct;
}

uint16_t xusb_vendor_id(const struct xusb *xusb)
{
	return  xusb->dev->descriptor.idVendor;
}

uint16_t xusb_product_id(const struct xusb *xusb)
{
	return  xusb->dev->descriptor.idProduct;
}

const struct xusb_spec *xusb_spec(const struct xusb *xusb)
{
	return xusb->spec;
}

int xusb_close(struct xusb *xusb)
{
	if (xusb) {
		if (xusb->handle) {
			assert(xusb->spec);
			assert(xusb->spec->name);
			DBG("Closing interface \"%s\"\n", xusb->spec->name);
			if (xusb->is_claimed) {
				if (usb_release_interface(xusb->handle,
					xusb->spec->my_interface_num) != 0)
					ERR("Releasing interface: usb: %s\n",
						usb_strerror());
				xusb->is_claimed = 0;
			}
			if (xusb->is_open) {
				if (usb_close(xusb->handle) != 0) {
					ERR("Closing device: usb: %s\n",
						usb_strerror());
				}
				xusb->is_open = 0;
			}
			xusb->handle = NULL;
		}
		xusb = NULL;
	}
	return 0;
}

int xusb_send(struct xusb *xusb, char *buf, int len, int timeout)
{
	int		ret;
	int		retries = 0;

	dump_packet(LOG_DEBUG, DBG_MASK, __func__, buf, len);
	if (EP_OUT(xusb) & USB_ENDPOINT_IN) {
		ERR("%s called with an input endpoint 0x%x\n",
			__func__, EP_OUT(xusb));
		return -EINVAL;
	}
retry_write:
	ret = usb_bulk_write(xusb->handle, EP_OUT(xusb), buf, len, timeout);
	if (ret < 0) {
		/*
		 * If the device was gone, it may be the
		 * result of renumeration. Ignore it.
		 */
		if (ret != -ENODEV) {
			ERR("bulk_write to endpoint 0x%x failed: (%d) %s\n",
				EP_OUT(xusb), ret, usb_strerror());
			dump_packet(LOG_ERR, DBG_MASK, "xusb_send[ERR]",
				buf, len);
			/*exit(2);*/
		} else {
			DBG("bulk_write to endpoint 0x%x got ENODEV\n",
				EP_OUT(xusb));
			xusb_close(xusb);
		}
		return ret;
	}
	if (!ret) {
		ERR("bulk_write to endpoint 0x%x short write[%d]: (%d)\n",
			EP_OUT(xusb), retries, ret);
		if (retries++ > MAX_RETRIES)
			return -EFAULT;
		usleep(100);
		goto retry_write;
	}
	if (ret != len) {
		ERR("bulk_write to endpoint 0x%x short write: (%d) %s\n",
			EP_OUT(xusb), ret, usb_strerror());
		dump_packet(LOG_ERR, DBG_MASK, "xusb_send[ERR]", buf, len);
		return -EFAULT;
	}
	return ret;
}

int xusb_recv(struct xusb *xusb, char *buf, size_t len, int timeout)
{
	int	ret;
	int	retries = 0;

	if (EP_IN(xusb) & USB_ENDPOINT_OUT) {
		ERR("%s called with an output endpoint 0x%x\n",
			__func__, EP_IN(xusb));
		return -EINVAL;
	}
retry_read:
	ret = usb_bulk_read(xusb->handle, EP_IN(xusb), buf, len, timeout);
	if (ret < 0) {
		DBG("bulk_read from endpoint 0x%x failed: (%d) %s\n",
			EP_IN(xusb), ret, usb_strerror());
		memset(buf, 0, len);
		return ret;
	}
	if (!ret) {
		ERR("bulk_read to endpoint 0x%x short read[%d]: (%d)\n",
			EP_IN(xusb), retries, ret);
		if (retries++ > MAX_RETRIES)
			return -EFAULT;
		usleep(100);
		goto retry_read;
	}
	dump_packet(LOG_DEBUG, DBG_MASK, __func__, buf, ret);
	return ret;
}

int xusb_flushread(struct xusb *xusb)
{
	char		tmpbuf[BUFSIZ];
	int		ret;

	DBG("starting...\n");
	memset(tmpbuf, 0, BUFSIZ);
	ret = xusb_recv(xusb, tmpbuf, BUFSIZ, 1);
	if (ret < 0 && ret != -ETIMEDOUT) {
		ERR("ret=%d\n", ret);
		return ret;
	} else if (ret > 0) {
		DBG("Got %d bytes:\n", ret);
		dump_packet(LOG_DEBUG, DBG_MASK, __func__, tmpbuf, ret);
	}
	return 0;
}

/*
 * Serialize calls to usb_find_busses()/usb_find_devices()
 */

static const key_t	SEM_KEY = 0x1a2b3c4d;
static int semid = -1;	/* Failure */

static void xusb_lock_usb()
{
	struct sembuf	sembuf;

	while (semid < 0) {
		/* Maybe it was already created? */
		semid = semget(SEM_KEY, 1, 0);
		if (semid < 0) {
			/* No, let's create ourselves */
			semid = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
			if (semid < 0) {
				/* Someone else won the race to create it */
				if (errno != ENOENT)
					ERR("%s: semget() failed: %s\n",
						__func__, strerror(errno));
				/* Retry */
				continue;
			}
			/* Initialize */
			if (semctl(semid, 0, SETVAL, 1) < 0)
				ERR("%s: SETVAL() failed: %s\n",
					__func__, strerror(errno));
		}
	}
	DBG("%d: LOCKING\n", getpid());
	sembuf.sem_num = 0;
	sembuf.sem_op = -1;
	sembuf.sem_flg = SEM_UNDO;
	if (semop(semid, &sembuf, 1) < 0)
		ERR("%s: semop() failed: %s\n", __func__, strerror(errno));
	DBG("%d: LOCKED\n", getpid());
}

static void xusb_unlock_usb()
{
	struct sembuf	sembuf;

	DBG("%d: UNLOCKING\n", getpid());
	sembuf.sem_num = 0;
	sembuf.sem_op = 1;
	sembuf.sem_flg = SEM_UNDO;
	if (semop(semid, &sembuf, 1) < 0)
		ERR("%s: semop() failed: %s\n", __func__, strerror(errno));
	DBG("%d: UNLOCKED\n", getpid());
}

static int		initizalized;

static void xusb_init()
{
	if (!initizalized) {
		xtalk_parse_options();
		if (!xtalk_option_no_lock())
			xusb_lock_usb();
		usb_init();
		usb_find_busses();
		usb_find_devices();
		initizalized = 1;
		if (!xtalk_option_no_lock())
			xusb_unlock_usb();
	}
}

/* XTALK option handling */
static int use_clear_halt = 1;
static int libusb_no_lock = 0;

static int xtalk_one_option(const char *option_string)
{
	if (strcmp(option_string, "use-clear-halt") == 0) {
		use_clear_halt = 1;
		return 0;
	}
	if (strcmp(option_string, "no-use-clear-halt") == 0) {
		use_clear_halt = 0;
		return 0;
	}
	if (strcmp(option_string, "no-lock") == 0) {
		libusb_no_lock = 1;
		return 0;
	}
	ERR("Unknown XTALK_OPTIONS content: '%s'\n", option_string);
	return -EINVAL;
}

int xtalk_option_use_clear_halt(void)
{
	return use_clear_halt;
}

int xtalk_option_no_lock(void)
{
	return libusb_no_lock;
}

int xtalk_parse_options(void)
{
	char *xtalk_options;
	char *saveptr;
	char *token;
	int ret;

	xtalk_options = getenv("XTALK_OPTIONS");
	if (!xtalk_options)
		return 0;
	token = strtok_r(xtalk_options, " \t", &saveptr);
	while (token) {
		ret = xtalk_one_option(token);
		if (ret < 0)
			return ret;
		token = strtok_r(NULL, " \t", &saveptr);
	}
	return 0;
}

