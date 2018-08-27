/*
 *  Hamlib Interface - toolbox header
 *  Copyright (c) 2000-2004 by Stephane Fillod
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

#ifndef _MISC_H
#define _MISC_H 1

#include <hamlib/rig.h>


/*
 * Carefull!! These marcos are NOT reentrant!
 * ie. they may not be executed atomically,
 * thus not ensure mutual exclusion.
 * Fix it when making Hamlib reentrant!  --SF
 */
#define Hold_Decode(rig) {(rig)->state.hold_decode = 1;}
#define Unhold_Decode(rig) {(rig)->state.hold_decode = 0;}

__BEGIN_DECLS

/*
 * Do a hex dump of the unsigned char array.
 */

void dump_hex(const unsigned char ptr[], size_t size);

/*
 * BCD conversion routines.
 * to_bcd converts a long long int to a little endian BCD array,
 * 	and return a pointer to this array.
 * from_bcd converts a little endian BCD array to long long int
 *  reprensentation, and return it.
 * bcd_len is the number of digits in the BCD array.
 */
extern HAMLIB_EXPORT(unsigned char *) to_bcd(unsigned char bcd_data[], unsigned long long freq, unsigned bcd_len);
extern HAMLIB_EXPORT(unsigned long long) from_bcd(const unsigned char bcd_data[], unsigned bcd_len);

/*
 * same as to_bcd and from_bcd, but in Big Endian mode
 */
extern HAMLIB_EXPORT(unsigned char *) to_bcd_be(unsigned char bcd_data[], unsigned long long freq, unsigned bcd_len);
extern HAMLIB_EXPORT(unsigned long long) from_bcd_be(const unsigned char bcd_data[], unsigned bcd_len);

extern HAMLIB_EXPORT(int) sprintf_freq(char *str, freq_t);

/* check if it's any of CR or LF */
#define isreturn(c) ((c) == 10 || (c) == 13)


/* needs config.h included beforehand in .c file */
#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

extern HAMLIB_EXPORT(int) rig_check_cache_timeout(const struct timeval *tv, int timeout);
extern HAMLIB_EXPORT(void) rig_force_cache_timeout(struct timeval *tv);


#ifdef PRId64
/** \brief printf(3) format to be used for long long (64bits) type */
#define PRIll PRId64
#else
#ifdef FBSD4
#define PRIll "qd"
#else
#define PRIll "lld"
#endif
#endif

#ifdef SCNd64
/** \brief scanf(3) format to be used for long long (64bits) type */
#define SCNll SCNd64
#else
#ifdef FBSD4
#define SCNll "qd"
#else
#define SCNll "lld"
#endif
#endif



__END_DECLS

#endif /* _MISC_H */
