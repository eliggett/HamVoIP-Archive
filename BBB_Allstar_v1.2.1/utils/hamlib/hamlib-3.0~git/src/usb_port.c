/*
 *  Hamlib Interface - USB communication low-level support
 *  Copyright (c) 2000-2011 by Stephane Fillod
 *
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/**
 * \addtogroup rig_internal
 * @{
 */

/**
 * \brief USB IO
 * \file usb_port.c
 *
 * doc todo: deal with defined(HAVE_LIBUSB)... quashing the doc process.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "hamlib/rig.h"

/*
 * Compile only if libusb is available
 */
#if defined(HAVE_LIBUSB) && defined(HAVE_USB_H)


#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <usb.h>
#include "usb_port.h"


/**
 * \brief Get ASCII string from USB descriptor
 * \param udh
 * \param index
 * \param langid
 * \param buf
 * \param buflen
 * \return status/len
 */
static int usbGetStringAscii(usb_dev_handle *udh, int index, int langid, char *buf, int buflen)
{
	char	buffer[256];
	int	rval, i;

	rval = usb_control_msg(udh, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR,
		  (USB_DT_STRING << 8) + index, langid, buffer, sizeof(buffer), 1000);

	if(rval < 0)
		return rval;

	if(buffer[1] != USB_DT_STRING)
		return 0;

	if((unsigned char)buffer[0] < rval)
		rval = (unsigned char)buffer[0];

	rval /= 2;

	/* lossy conversion to ISO Latin1 */
	for(i=1;i<rval;i++) {
		if(i > buflen)			/* destination buffer overflow */
			break;
		buf[i - 1] = buffer[2 * i];
		if(buffer[2 * i + 1] != 0)	/* outside of ISO Latin1 range */
			buf[i - 1] = '?';
	}

	buf[i - 1] = 0;
	return i - 1;
}

/**
 * \brief Find and open USB device
 * \param port
 * \return usb_handle
 */
static struct usb_dev_handle *find_and_open_device(const hamlib_port_t *port)
{
	struct usb_bus *bus;
	struct usb_device *dev;
	struct usb_dev_handle *udh;
	char	string[256];
	int	len;

	rig_debug(RIG_DEBUG_VERBOSE, "%s: looking for device %04x:%04x...",
		__func__, port->parm.usb.vid, port->parm.usb.pid);

	for (bus = usb_get_busses(); bus != NULL; bus = bus->next) {
		for (dev = bus->devices; dev != NULL; dev = dev->next) {

			rig_debug(RIG_DEBUG_VERBOSE, " %04x:%04x,",
				dev->descriptor.idVendor, dev->descriptor.idProduct);
			if (dev->descriptor.idVendor == port->parm.usb.vid &&
				dev->descriptor.idProduct == port->parm.usb.pid) {

				/* we need to open the device in order to query strings */
				udh = usb_open(dev);
				if (!udh) {
					rig_debug(RIG_DEBUG_WARN, "%s: Warning: Cannot open USB device: %s\n", __func__, usb_strerror());
					continue;
				}

				/* now check whether the names match: */
				if (port->parm.usb.vendor_name) {
					len = usbGetStringAscii(udh, dev->descriptor.iManufacturer, 0x0409, string, sizeof(string));
					if (len < 0) {
						rig_debug(RIG_DEBUG_WARN, "Warning: cannot query manufacturer for USB device: %s\n", usb_strerror());
						usb_close(udh);
						continue;
					}

					rig_debug(RIG_DEBUG_VERBOSE, " vendor >%s<", string);

					if (strcmp(string, port->parm.usb.vendor_name) != 0) {
						rig_debug(RIG_DEBUG_WARN, "%s: Warning: Vendor name string mismatch!\n", __func__);
						usb_close(udh);
						continue;
					}
				}

				if (port->parm.usb.product) {
					len = usbGetStringAscii(udh, dev->descriptor.iProduct, 0x0409, string, sizeof(string));
					if (len < 0) {
						rig_debug(RIG_DEBUG_WARN, "Warning: cannot query product for USB device: %s\n", usb_strerror());
						usb_close(udh);
						continue;
					}

					rig_debug(RIG_DEBUG_VERBOSE, " product >%s<", string);

					if (strcmp(string, port->parm.usb.product) != 0) {
						/* Now testing with strncasecmp() for case insensitive
						 * match.  Updating firmware on FUNcube Dongle to v18f resulted
						 * in product string changing from "FunCube Dongle" to
						 * "FUNcube Dongle".  As new dongles are shipped with
						 * older firmware, both product strings are valid.  Sigh...
						 */
						if (strncasecmp(string, port->parm.usb.product, sizeof(port->parm.usb.product - 1)) != 0) {
							rig_debug(RIG_DEBUG_WARN, "%s: Warning: Product string mismatch!\n", __func__);
							usb_close(udh);
							continue;
						}
					}
				}

				rig_debug(RIG_DEBUG_VERBOSE, " -> found\n");
				return udh;
			}
		}
	}

	rig_debug(RIG_DEBUG_VERBOSE, " -> not found\n");
	return NULL;		/* not found */
}


/**
 * \brief Open hamlib_port of USB device
 * \param port
 * \return status
 */
int usb_port_open(hamlib_port_t *port)
{
	struct usb_dev_handle *udh;
	char *p, *q;

	usb_init ();	/* usb library init */
	if (usb_find_busses () < 0)
		rig_debug(RIG_DEBUG_ERR, "%s: usb_find_busses failed %s\n",
			__func__, usb_strerror());
	if (usb_find_devices() < 0)
		rig_debug(RIG_DEBUG_ERR, "%s: usb_find_devices failed %s\n",
			__func__, usb_strerror());

	p = port->pathname;
	q = strchr(p, ':');
	if (q) {
		if (q != p+1)
			port->parm.usb.vid = strtol(q, NULL, 16);
		p = q+1;
		q = strchr(p, ':');
		if (q) {
			if (q != p+1)
				port->parm.usb.pid = strtol(q, NULL, 16);
			p = q+1;
			q = strchr(p, ':');
			if (q) {
				if (q != p+1)
					port->parm.usb.vendor_name = q;
				p = q+1;
				q = strchr(p, ':');
				if (q) {
					if (q != p+1)
						port->parm.usb.product = q;
				}
			}
		}
	}

	udh = find_and_open_device(port);
	if (udh == 0)
		return -RIG_EIO;

#ifdef LIBUSB_HAS_GET_DRIVER_NP
	/* Try to detach ftdi_sio kernel module
	 * This should be performed only for devices using
	 * USB-serial converters (like FTDI chips), for other
	 * devices this may cause problems, so do not do it.
	 */
	char dname[32] = {0};
	int retval = usb_get_driver_np(udh, port->parm.usb.iface, dname, 31);
	if (!retval) {
		/* Try to detach ftdi_sio kernel module
		 * Returns ENODATA if driver is not loaded.
		 * Don't check return value, let it fail later.
		 */
		usb_detach_kernel_driver_np(udh, port->parm.usb.iface);
	}
#endif

	if (port->parm.usb.iface >= 0) {

#ifdef _WIN32
		if (port->parm.usb.conf >= 0 &&
			usb_set_configuration (udh, port->parm.usb.conf) < 0) {
			rig_debug(RIG_DEBUG_ERR, "%s: usb_set_configuration: failed conf %d: %s\n",
				__func__, port->parm.usb.conf, usb_strerror());
			usb_close (udh);
			return -RIG_EIO;
		}
#endif

		rig_debug(RIG_DEBUG_VERBOSE, "%s: claiming %d\n", __func__, port->parm.usb.iface);

		if (usb_claim_interface (udh, port->parm.usb.iface) < 0) {
			rig_debug(RIG_DEBUG_ERR, "%s:usb_claim_interface: failed interface %d: %s\n",
				__func__,port->parm.usb.iface, usb_strerror());
			usb_close (udh);
			return -RIG_EIO;
		}

#if 0
		if (usb_set_altinterface (udh, port->parm.usb.alt) < 0) {
			fprintf (stderr, "%s:usb_set_alt_interface: failed: %s\n", __func__,
				usb_strerror());
			usb_release_interface (udh, port->parm.usb.iface);
			usb_close (udh);
			return -RIG_EIO;
		}
#endif

	}

	port->handle = (void*) udh;

	return RIG_OK;
}

/**
 * \brief Close hamlib_port of USB device
 * \param port
 * \return status
 */
int usb_port_close(hamlib_port_t *port)
{
	struct usb_dev_handle *udh = port->handle;

	usb_release_interface (udh, port->parm.usb.iface);

	/* we're assuming that closing an interface automatically releases it. */
	return usb_close (udh) == 0 ? RIG_OK : -RIG_EIO;
}

#else

int usb_port_open(hamlib_port_t *port)
{
	return -RIG_ENAVAIL;
}

int usb_port_close(hamlib_port_t *port)
{
	return -RIG_ENAVAIL;
}

#endif	/* defined(HAVE_LIBUSB) && defined(HAVE_USB_H) */

/** @} */
