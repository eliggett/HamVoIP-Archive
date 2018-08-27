/*
 *  Hamlib KIT backend - main file
 *  Copyright (c) 2004-2012 by Stephane Fillod
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */

#include "hamlib/rig.h"
#include "register.h"

#include "kit.h"
#include "usrp_impl.h"

/*
 * initrigs_kit is called by rig_backend_load
 */
DECLARE_INITRIG_BACKEND(kit)
{
	rig_debug(RIG_DEBUG_VERBOSE, "kit: _init called\n");

	rig_register(&elektor304_caps);
	rig_register(&drt1_caps);
	rig_register(&dds60_caps);
	rig_register(&miniVNA_caps);
	rig_register(&hiqsdr_caps);

#if (defined(HAVE_LIBUSB) && defined(HAVE_USB_H))
	rig_register(&si570avrusb_caps);
	rig_register(&si570picusb_caps);
	rig_register(&si570peaberry1_caps);
	rig_register(&si570peaberry2_caps);
	rig_register(&funcube_caps);
	rig_register(&fifisdr_caps);
	rig_register(&fasdr_caps);
	rig_register(&funcubeplus_caps);
#endif
#if (defined(HAVE_LIBUSB) && defined(HAVE_USB_H)) || defined(_WIN32)
    /* rigs with alternate DLL support on Win32 */
	rig_register(&dwt_caps);
	rig_register(&elektor507_caps);
#endif

#ifdef HAVE_USRP
	//rig_register(&usrp0_caps);
	rig_register(&usrp_caps);
#endif

	return RIG_OK;
}

/*
 * initrots_kit is called by rot_backend_load
 */
DECLARE_INITROT_BACKEND(kit)
{
	rig_debug(RIG_DEBUG_VERBOSE, "kit: _init called\n");

	rot_register(&pcrotor_caps);

	return RIG_OK;
}


