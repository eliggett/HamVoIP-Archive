/*
 *  Hamlib Interface - CM108 HID communication low-level support
 *  Copyright (c) 2000-2012 by Stephane Fillod
 *  Copyright (c) 2011 by Andrew Errington
 *  CM108 detection code Copyright (c) Thomas Sailer used with permission
 *
 *   This library is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of
 *   the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/**
 * \addtogroup rig_internal
 * @{
 */

/**
 * \brief CM108 GPIO
 * \file cm108.c
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include "par_nt.h"
#endif
#ifdef HAVE_WINIOCTL_H
#include <winioctl.h>
#endif
#ifdef HAVE_WINBASE_H
#include <winbase.h>
#endif

#ifdef HAVE_LINUX_HIDRAW_H
#include <linux/hidraw.h>
#endif

#include "hamlib/rig.h"
#include "cm108.h"


/**
 * \brief Open CM108 HID port (/dev/hidrawX)
 * \param port
 * \return file descriptor
 */

int cm108_open(hamlib_port_t *port)
{
	int fd;

	rig_debug(RIG_DEBUG_VERBOSE,"cm108:cm108_open called \n");


	if (!port->pathname)
		return -RIG_EINVAL;

	fd = open(port->pathname, O_RDWR);

	if (fd < 0) {
		rig_debug(RIG_DEBUG_ERR, "cm108:Opening device \"%s\": %s\n", port->pathname, strerror(errno));
		return -RIG_EIO;
	}

#ifdef HAVE_LINUX_HIDRAW_H
	// CM108 detection copied from Thomas Sailer's soundmodem code

	rig_debug(RIG_DEBUG_VERBOSE,"cm108:Checking for cm108 (or compatible) device \n");

	struct hidraw_devinfo hiddevinfo;

	if (!ioctl(fd, HIDIOCGRAWINFO, &hiddevinfo)
	&&
	  (
	    (hiddevinfo.vendor == 0x0d8c	// CM108/109/119
		&& hiddevinfo.product >= 0x0008
		&& hiddevinfo.product <= 0x000f
	    )
	    ||
	    (hiddevinfo.vendor == 0x0c76 &&	// SSS1621/23
		(hiddevinfo.product == 0x1605 ||
		hiddevinfo.product == 0x1607 ||
		hiddevinfo.product == 0x160b)
	    )
	  )
	)
	{
		rig_debug(RIG_DEBUG_VERBOSE,"cm108:cm108 compatible device detected \n");
	}
	else
	{
		close(fd);
		rig_debug(RIG_DEBUG_VERBOSE,"cm108:No cm108 (or compatible) device detected \n");
		return -RIG_EINVAL;
	}
#endif

	port->fd = fd;
	return fd;
}

/**
 * \brief Close CM108 HID port
 * \param port
 */
int cm108_close(hamlib_port_t *port)
{
        rig_debug(RIG_DEBUG_VERBOSE,"cm108:cm108_close called \n");

	return close(port->fd);
}

/**
 * \brief Set or unset Push to talk bit on CM108 GPIO
 * \param p
 * \param pttx RIG_PTT_ON --> Set PTT
 * \return RIG_OK or < 0 error
 */
int cm108_ptt_set(hamlib_port_t *p, ptt_t pttx)
{

        rig_debug(RIG_DEBUG_VERBOSE,"cm108:cm108_ptt_set called \n");

	// For a CM108 USB audio device PTT is wired up to one of the GPIO
	// pins.  Usually this is GPIO3 (bit 2 of the GPIO register) because it
	// is on the corner of the chip package (pin 13) so it's easily accessible.
	// Some CM108 chips are epoxy-blobbed onto the PCB, so no GPIO
	// pins are accessible.  The SSS1623 chips have a different pinout, so
	// we make the GPIO bit number configurable.

	switch(p->type.ptt) {
	case RIG_PTT_CM108:
		{

		// Build a packet for CM108 HID to turn GPIO bit on or off.
		// Packet is 4 bytes, preceded by a 'report number' byte
		// 0x00 report number
		// Write data packet (from CM108 documentation)
		// byte 0: 00xx xxxx     Write GPIO
		// byte 1: xxxx dcba     GPIO3-0 output values (1=high)
		// byte 2: xxxx dcba     GPIO3-0 data-direction register (1=output)
		// byte 3: xxxx xxxx     SPDIF

		rig_debug(RIG_DEBUG_VERBOSE,"cm108:cm108_ptt_set bit number %d to state %d\n",
				p->parm.cm108.ptt_bitnum, (pttx == RIG_PTT_ON) ? 1 : 0);

		char out_rep[] = {
			0x00, // report number
			// HID output report
			0x00,
			(pttx == RIG_PTT_ON) ? (1 << p->parm.cm108.ptt_bitnum) : 0, // set GPIO
			1 << p->parm.cm108.ptt_bitnum, // Data direction register (1=output)
			0x00
		};

		ssize_t nw;

		if (p->fd == -1)
			return -RIG_EINVAL;

		// Send the HID packet
		nw = write(p->fd, out_rep, sizeof(out_rep));
		if (nw < 0) {
			return -RIG_EIO;
		}

		return RIG_OK;
		}
	default:
		rig_debug(RIG_DEBUG_ERR,"Unsupported PTT type %d\n",
						p->type.ptt);
		return -RIG_EINVAL;
	}
	return RIG_OK;
}

/**
 * \brief Get state of Push to Talk from CM108 GPIO
 * \param p
 * \param pttx return value (must be non NULL)
 * \return RIG_OK or < 0 error
 */
int cm108_ptt_get(hamlib_port_t *p, ptt_t *pttx)
{
        rig_debug(RIG_DEBUG_VERBOSE,"cm108:cm108_ptt_get called \n");

	switch(p->type.ptt) {
	case RIG_PTT_CM108:
		{
		int status;
		return -RIG_ENIMPL;
		return status;
		}
	default:
		rig_debug(RIG_DEBUG_ERR,"Unsupported PTT type %d\n",
						p->type.ptt);
		return -RIG_ENAVAIL;
	}
	return RIG_OK;
}

/**
 * \brief get Data Carrier Detect (squelch) from CM108 GPIO
 * \param p
 * \param dcdx return value (Must be non NULL)
 * \return RIG_OK or < 0 error
 */
int cm108_dcd_get(hamlib_port_t *p, dcd_t *dcdx)
{
        rig_debug(RIG_DEBUG_VERBOSE,"cm108:cm108_dcd_get called \n");

	// On the CM108 and compatible chips the squelch line on the radio is
	// wired to Volume Down input pin.  The state of this pin is reported
	// in HID messages from the CM108 device, but I am not sure how
	// to query this state on demand.

	switch(p->type.dcd) {
	case RIG_DCD_CM108:
		{
		int status;
		return -RIG_ENIMPL;
		return status;
		}
	default:
		rig_debug(RIG_DEBUG_ERR,"Unsupported DCD type %d\n",
						p->type.dcd);
		return -RIG_ENAVAIL;
	}
	return RIG_OK;
}

/** @} */
