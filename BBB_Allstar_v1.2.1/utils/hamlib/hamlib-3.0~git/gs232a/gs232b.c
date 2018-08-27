/*
 *  Hamlib Rotator backend - GS-232B
 *  Copyright (c) 2001-2012 by Stephane Fillod
 *                 (c) 2010 by Kobus Botha
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <math.h>

#include "hamlib/rotator.h"
#include "serial.h"
#include "misc.h"
#include "register.h"

#include "gs232a.h"

#define EOM "\r"
#define REPLY_EOM "\r\n"

#define BUFSZ 64

/**
 * gs232b_transaction
 *
 * cmdstr - Command to be sent to the rig.
 * data - Buffer for reply string.  Can be NULL, indicating that no reply is
 *        is needed, but answer will still be read.
 * data_len - in: Size of buffer. It is the caller's responsibily to provide
 *            a large enough buffer for all possible replies for a command.
 *
 * returns:
 *   RIG_OK  -  if no error occured.
 *   RIG_EIO  -  if an I/O error occured while sending/receiving data.
 *   RIG_ETIMEOUT  -  if timeout expires without any characters received.
 *   RIG_REJECTED  -  if a negative acknowledge was received or command not
 *                    recognized by rig.
 */
static int
gs232b_transaction (ROT *rot, const char *cmdstr,
				char *data, size_t data_len)
{
    struct rot_state *rs;
    int retval;
    int retry_read = 0;
    char replybuf[BUFSZ];

    rs = &rot->state;

transaction_write:

    serial_flush(&rs->rotport);

    if (cmdstr) {
        retval = write_block(&rs->rotport, cmdstr, strlen(cmdstr));
        if (retval != RIG_OK)
            goto transaction_quit;
    }

    /* Always read the reply to know whether the cmd went OK */
    if (!data)
        data = replybuf;
    if (!data_len)
        data_len = BUFSZ;

    memset(data,0,data_len);
    retval = read_string(&rs->rotport, data, data_len, REPLY_EOM, strlen(REPLY_EOM));
    if (retval < 0) {
        if (retry_read++ < rot->state.rotport.retry)
            goto transaction_write;
        goto transaction_quit;
    }

#if 0
    /* Check that command termination is correct */

    if (strchr(REPLY_EOM, data[strlen(data)-1])==NULL) {
        rig_debug(RIG_DEBUG_ERR, "%s: Command is not correctly terminated '%s'\n", __FUNCTION__, data);
        if (retry_read++ < rig->state.rotport.retry)
            goto transaction_write;
        retval = -RIG_EPROTO;
        goto transaction_quit;
    }
#endif


    if (data[0] == '?') {
	    /* Invalid command */
	    rig_debug(RIG_DEBUG_VERBOSE, "%s: Error for '%s': '%s'\n",
			    __FUNCTION__, cmdstr, data);
	    retval = -RIG_EPROTO;
	    goto transaction_quit;
    }

    retval = RIG_OK;
transaction_quit:
    return retval;
}


static int
gs232b_rot_set_position(ROT *rot, azimuth_t az, elevation_t el)
{
    char cmdstr[64];
    int retval;
    unsigned u_az, u_el;

    rig_debug(RIG_DEBUG_TRACE, "%s called: %f %f\n", __FUNCTION__, az, el);

    u_az = (unsigned)rint(az);
    u_el = (unsigned)rint(el);

    sprintf(cmdstr, "W%03u %03u" EOM, u_az, u_el);
    retval = gs232b_transaction(rot, cmdstr, NULL, 0);

    if (retval != RIG_OK) {
        return retval;
    }

    return RIG_OK;
}

static int
gs232b_rot_get_position(ROT *rot, azimuth_t *az, elevation_t *el)
{
    char posbuf[32];
    int retval, int_az, int_el;

    rig_debug(RIG_DEBUG_TRACE, "%s called\n", __FUNCTION__);

    retval = gs232b_transaction(rot, "C2" EOM, posbuf, sizeof(posbuf));
    if (retval != RIG_OK || strlen(posbuf) < 10) {
        return retval < 0 ? retval : -RIG_EPROTO;
    }

    /* parse "AZ=aaa   EL=eee" */

    /* With the format string containing a space character as one of the
     * directives, any amount of space is matched, including none in the input.
     */
    if (sscanf(posbuf, "AZ=%d EL=%d", &int_az, &int_el) != 2) {
        rig_debug(RIG_DEBUG_ERR, "%s: wrong reply '%s'\n", __FUNCTION__, posbuf);
        return -RIG_EPROTO;
    }
    *az = (azimuth_t)int_az;
    *el = (elevation_t)int_el;

    rig_debug(RIG_DEBUG_TRACE, "%s: (az, el) = (%.1f, %.1f)\n",
		   __FUNCTION__, *az, *el);

    return RIG_OK;
}

static int
gs232b_rot_stop(ROT *rot)
{
    int retval;

    rig_debug(RIG_DEBUG_TRACE, "%s called\n", __FUNCTION__);

    /* All Stop */
    retval = gs232b_transaction(rot, "S" EOM, NULL, 0);
    if (retval != RIG_OK)
        return retval;

    return RIG_OK;
}


static int
gs232b_rot_move(ROT *rot, int direction, int speed)
{
    char cmdstr[24];
    int retval;
    unsigned x_speed;

    rig_debug(RIG_DEBUG_TRACE, "%s called %d %d\n", __FUNCTION__,
		    direction, speed);

    x_speed = (3*speed)/100 + 1;

    /* between 1 (slowest) and 4 (fastest) */
    sprintf(cmdstr, "X%u" EOM, x_speed);
    retval = gs232b_transaction(rot, cmdstr, NULL, 0);
    if (retval != RIG_OK)
        return retval;

    switch (direction) {
    case ROT_MOVE_UP:       /* Elevation increase */
        sprintf(cmdstr, "U" EOM);
        break;
    case ROT_MOVE_DOWN:     /* Elevation decrease */
        sprintf(cmdstr, "D" EOM);
        break;
    case ROT_MOVE_LEFT:     /* Azimuth decrease */
        sprintf(cmdstr, "L" EOM);
        break;
    case ROT_MOVE_RIGHT:    /* Azimuth increase */
        sprintf(cmdstr, "R" EOM);
        break;
    default:
        rig_debug(RIG_DEBUG_ERR,"%s: Invalid direction value! (%d)\n",
			__FUNCTION__, direction);
        return -RIG_EINVAL;
    }

    retval = gs232b_transaction(rot, cmdstr, NULL, 0);
    if (retval != RIG_OK)
        return retval;

    return RIG_OK;
}

/* ************************************************************************* */
/*
 * Generic GS232B rotator capabilities.
 */

const struct rot_caps gs232b_rot_caps = {
  .rot_model =      ROT_MODEL_GS232B,
  .model_name =     "GS-232B",
  .mfg_name =       "Yaesu",
  .version =        "0.2",
  .copyright = 	    "LGPL",
  .status =         RIG_STATUS_BETA,
  .rot_type =       ROT_TYPE_OTHER,
  .port_type =      RIG_PORT_SERIAL,
  .serial_rate_min =   150,
  .serial_rate_max =   9600,
  .serial_data_bits =  8,
  .serial_stop_bits =  1,
  .serial_parity =  RIG_PARITY_NONE,
  .serial_handshake =  RIG_HANDSHAKE_NONE,
  .write_delay =  0,
  .post_write_delay =  0,
  .timeout =  400,
  .retry =  3,

  .min_az = 	0.0,
  .max_az =  	450.0,	/* vary according to rotator type */
  .min_el = 	0.0,
  .max_el =  	180.0, /* requires G-5400B, G-5600B, G-5500, or G-500/G-550 */

  .get_position =  gs232b_rot_get_position,
  .set_position =  gs232b_rot_set_position,
  .stop = 	       gs232b_rot_stop,
  .move =          gs232b_rot_move,
};

/* end of file */

