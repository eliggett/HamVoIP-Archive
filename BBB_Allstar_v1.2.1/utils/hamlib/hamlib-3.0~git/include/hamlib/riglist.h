/*
 *  Hamlib Interface - list of known rigs
 *  Copyright (c) 2000-2003 by Frank Singleton
 *  Copyright (c) 2000-2012 by Stephane Fillod
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

#ifndef _RIGLIST_H
#define _RIGLIST_H 1

#define RIG_MAKE_MODEL(a,b) ((a)*100+(b))
#define RIG_BACKEND_NUM(a) ((a)/100)

/*! \file riglist.h
 *  \brief Hamlib rig(radio) model definitions.
 *
 *  This file contains rig model definitions for the Hamlib rig API.
 *  Each distinct rig type has a unique model number (ID) and is used
 *  by hamlib to identify and distinguish between the different hardware drivers.
 *  The exact model numbers can be acquired using the macros in this
 *  file. To obtain a list of supported rig branches, one can use the statically
 *  defined RIG_BACKEND_LIST macro. To obtain a full list of supported rig (including
 *  each model in every branch), the foreach_opened_rig() API function can be used.
 *
 *  The model number, or ID, is used to tell hamlib, which rig the client whishes to
 *  use. It is done with the rig_init() API call.
 */

#define RIG_MODEL_NONE 0

/*! \def RIG_MODEL_DUMMY
 *  \brief A macro that returns the model number for the dummy backend.
 *
 *  The dummy backend, as the name suggests, is a backend which performs
 *  no hardware operations and always behaves as one would expect. It can
 *  be thought of as a hardware simulator and is very usefull for testing
 *  client applications.
 */
#define RIG_DUMMY 0
#define RIG_BACKEND_DUMMY "dummy"
#define RIG_MODEL_DUMMY RIG_MAKE_MODEL(RIG_DUMMY, 1)
#define RIG_MODEL_NETRIGCTL RIG_MAKE_MODEL(RIG_DUMMY, 2)
#define RIG_MODEL_ARMSTRONG RIG_MAKE_MODEL(RIG_DUMMY, 3)

	/*
	 * Yaesu
	 */
#define RIG_YAESU 1
#define RIG_BACKEND_YAESU "yaesu"
#define RIG_MODEL_FT847 RIG_MAKE_MODEL(RIG_YAESU, 1)
#define RIG_MODEL_FT1000 RIG_MAKE_MODEL(RIG_YAESU, 2)
#define RIG_MODEL_FT1000D RIG_MAKE_MODEL(RIG_YAESU, 3)
#define RIG_MODEL_FT1000MPMKV RIG_MAKE_MODEL(RIG_YAESU, 4)
#define RIG_MODEL_FT747 RIG_MAKE_MODEL(RIG_YAESU, 5)
#define RIG_MODEL_FT757 RIG_MAKE_MODEL(RIG_YAESU, 6)
#define RIG_MODEL_FT757GXII RIG_MAKE_MODEL(RIG_YAESU, 7)
#define RIG_MODEL_FT575 RIG_MAKE_MODEL(RIG_YAESU, 8)
#define RIG_MODEL_FT767 RIG_MAKE_MODEL(RIG_YAESU, 9)
#define RIG_MODEL_FT736R RIG_MAKE_MODEL(RIG_YAESU, 10)
#define RIG_MODEL_FT840 RIG_MAKE_MODEL(RIG_YAESU, 11)
#define RIG_MODEL_FT820 RIG_MAKE_MODEL(RIG_YAESU, 12)
#define RIG_MODEL_FT900 RIG_MAKE_MODEL(RIG_YAESU, 13)
#define RIG_MODEL_FT920 RIG_MAKE_MODEL(RIG_YAESU, 14)
#define RIG_MODEL_FT890 RIG_MAKE_MODEL(RIG_YAESU, 15)
#define RIG_MODEL_FT990 RIG_MAKE_MODEL(RIG_YAESU, 16)
#define RIG_MODEL_FRG100 RIG_MAKE_MODEL(RIG_YAESU, 17)
#define RIG_MODEL_FRG9600 RIG_MAKE_MODEL(RIG_YAESU, 18)
#define RIG_MODEL_FRG8800 RIG_MAKE_MODEL(RIG_YAESU, 19)
#define RIG_MODEL_FT817 RIG_MAKE_MODEL(RIG_YAESU, 20)
#define RIG_MODEL_FT100 RIG_MAKE_MODEL(RIG_YAESU, 21)
#define RIG_MODEL_FT857 RIG_MAKE_MODEL(RIG_YAESU, 22)
#define RIG_MODEL_FT897 RIG_MAKE_MODEL(RIG_YAESU, 23)
#define RIG_MODEL_FT1000MP RIG_MAKE_MODEL(RIG_YAESU, 24)
#define RIG_MODEL_FT1000MPMKVFLD RIG_MAKE_MODEL(RIG_YAESU, 25)
#define RIG_MODEL_VR5000 RIG_MAKE_MODEL(RIG_YAESU, 26)
#define RIG_MODEL_FT450 RIG_MAKE_MODEL(RIG_YAESU, 27)
#define RIG_MODEL_FT950 RIG_MAKE_MODEL(RIG_YAESU, 28)
#define RIG_MODEL_FT2000 RIG_MAKE_MODEL(RIG_YAESU, 29)
#define RIG_MODEL_FT9000 RIG_MAKE_MODEL(RIG_YAESU, 30)
#define RIG_MODEL_FT980 RIG_MAKE_MODEL(RIG_YAESU, 31)
#define RIG_MODEL_FTDX5000 RIG_MAKE_MODEL(RIG_YAESU, 32)
#define RIG_MODEL_VX1700 RIG_MAKE_MODEL(RIG_YAESU, 33)

	/*
	 * Kenwood
	 */
#define RIG_KENWOOD 2
#define RIG_BACKEND_KENWOOD "kenwood"
#define RIG_MODEL_TS50 RIG_MAKE_MODEL(RIG_KENWOOD, 1)
#define RIG_MODEL_TS440 RIG_MAKE_MODEL(RIG_KENWOOD, 2)
#define RIG_MODEL_TS450S RIG_MAKE_MODEL(RIG_KENWOOD, 3)
#define RIG_MODEL_TS570D RIG_MAKE_MODEL(RIG_KENWOOD, 4)
#define RIG_MODEL_TS690S RIG_MAKE_MODEL(RIG_KENWOOD, 5)
#define RIG_MODEL_TS711 RIG_MAKE_MODEL(RIG_KENWOOD, 6)
#define RIG_MODEL_TS790 RIG_MAKE_MODEL(RIG_KENWOOD, 7)
#define RIG_MODEL_TS811 RIG_MAKE_MODEL(RIG_KENWOOD, 8)
#define RIG_MODEL_TS850 RIG_MAKE_MODEL(RIG_KENWOOD, 9)
#define RIG_MODEL_TS870S RIG_MAKE_MODEL(RIG_KENWOOD, 10)
#define RIG_MODEL_TS940 RIG_MAKE_MODEL(RIG_KENWOOD, 11)
#define RIG_MODEL_TS950 RIG_MAKE_MODEL(RIG_KENWOOD, 12)
#define RIG_MODEL_TS950SDX RIG_MAKE_MODEL(RIG_KENWOOD, 13)
#define RIG_MODEL_TS2000 RIG_MAKE_MODEL(RIG_KENWOOD, 14)
#define RIG_MODEL_R5000 RIG_MAKE_MODEL(RIG_KENWOOD, 15)
#define RIG_MODEL_TS570S RIG_MAKE_MODEL(RIG_KENWOOD, 16)
#define RIG_MODEL_THD7A RIG_MAKE_MODEL(RIG_KENWOOD, 17)
#define RIG_MODEL_THD7AG RIG_MAKE_MODEL(RIG_KENWOOD, 18)
#define RIG_MODEL_THF6A RIG_MAKE_MODEL(RIG_KENWOOD, 19)
#define RIG_MODEL_THF7E RIG_MAKE_MODEL(RIG_KENWOOD, 20)
#define RIG_MODEL_K2 RIG_MAKE_MODEL(RIG_KENWOOD, 21)
#define RIG_MODEL_TS930 RIG_MAKE_MODEL(RIG_KENWOOD, 22)
#define RIG_MODEL_THG71 RIG_MAKE_MODEL(RIG_KENWOOD, 23)
#define RIG_MODEL_TS680S RIG_MAKE_MODEL(RIG_KENWOOD, 24)
#define RIG_MODEL_TS140S RIG_MAKE_MODEL(RIG_KENWOOD, 25)
#define RIG_MODEL_TMD700 RIG_MAKE_MODEL(RIG_KENWOOD, 26)
#define RIG_MODEL_TMV7 RIG_MAKE_MODEL(RIG_KENWOOD, 27)
#define RIG_MODEL_TS480 RIG_MAKE_MODEL(RIG_KENWOOD, 28)
#define RIG_MODEL_K3 RIG_MAKE_MODEL(RIG_KENWOOD, 29)
#define RIG_MODEL_TRC80 RIG_MAKE_MODEL(RIG_KENWOOD, 30)
#define RIG_MODEL_TS590S RIG_MAKE_MODEL(RIG_KENWOOD, 31)
#define RIG_MODEL_TRANSFOX RIG_MAKE_MODEL(RIG_KENWOOD, 32)  /* SigFox Transfox */
#define RIG_MODEL_THD72A RIG_MAKE_MODEL(RIG_KENWOOD, 33)
#define RIG_MODEL_TMD710 RIG_MAKE_MODEL(RIG_KENWOOD, 34)
#define RIG_MODEL_TMV71 RIG_MAKE_MODEL(RIG_KENWOOD, 35)
#define RIG_MODEL_F6K RIG_MAKE_MODEL(RIG_KENWOOD, 36) /* Flex 6000 Series */

	/*
	 * Icom
	 */
#define RIG_ICOM 3
#define RIG_BACKEND_ICOM "icom"
#define RIG_MODEL_IC1271 RIG_MAKE_MODEL(RIG_ICOM, 1)
#define RIG_MODEL_IC1275 RIG_MAKE_MODEL(RIG_ICOM, 2)
#define RIG_MODEL_IC271 RIG_MAKE_MODEL(RIG_ICOM, 3)
#define RIG_MODEL_IC275 RIG_MAKE_MODEL(RIG_ICOM, 4)
#define RIG_MODEL_IC375 RIG_MAKE_MODEL(RIG_ICOM, 5)
#define RIG_MODEL_IC471 RIG_MAKE_MODEL(RIG_ICOM, 6)
#define RIG_MODEL_IC475 RIG_MAKE_MODEL(RIG_ICOM, 7)
#define RIG_MODEL_IC575 RIG_MAKE_MODEL(RIG_ICOM, 8)
#define RIG_MODEL_IC706 RIG_MAKE_MODEL(RIG_ICOM, 9)
#define RIG_MODEL_IC706MKII RIG_MAKE_MODEL(RIG_ICOM, 10)
#define RIG_MODEL_IC706MKIIG RIG_MAKE_MODEL(RIG_ICOM, 11)
#define RIG_MODEL_IC707 RIG_MAKE_MODEL(RIG_ICOM, 12)
#define RIG_MODEL_IC718 RIG_MAKE_MODEL(RIG_ICOM, 13)
#define RIG_MODEL_IC725 RIG_MAKE_MODEL(RIG_ICOM, 14)
#define RIG_MODEL_IC726 RIG_MAKE_MODEL(RIG_ICOM, 15)
#define RIG_MODEL_IC728 RIG_MAKE_MODEL(RIG_ICOM, 16)
#define RIG_MODEL_IC729 RIG_MAKE_MODEL(RIG_ICOM, 17)
#define RIG_MODEL_IC731 RIG_MAKE_MODEL(RIG_ICOM, 18)
#define RIG_MODEL_IC735 RIG_MAKE_MODEL(RIG_ICOM, 19)
#define RIG_MODEL_IC736 RIG_MAKE_MODEL(RIG_ICOM, 20)
#define RIG_MODEL_IC737 RIG_MAKE_MODEL(RIG_ICOM, 21)
#define RIG_MODEL_IC738 RIG_MAKE_MODEL(RIG_ICOM, 22)
#define RIG_MODEL_IC746 RIG_MAKE_MODEL(RIG_ICOM, 23)
#define RIG_MODEL_IC751 RIG_MAKE_MODEL(RIG_ICOM, 24)
#define RIG_MODEL_IC751A RIG_MAKE_MODEL(RIG_ICOM, 25)
#define RIG_MODEL_IC756 RIG_MAKE_MODEL(RIG_ICOM, 26)
#define RIG_MODEL_IC756PRO RIG_MAKE_MODEL(RIG_ICOM, 27)
#define RIG_MODEL_IC761 RIG_MAKE_MODEL(RIG_ICOM, 28)
#define RIG_MODEL_IC765 RIG_MAKE_MODEL(RIG_ICOM, 29)
#define RIG_MODEL_IC775 RIG_MAKE_MODEL(RIG_ICOM, 30)
#define RIG_MODEL_IC781 RIG_MAKE_MODEL(RIG_ICOM, 31)
#define RIG_MODEL_IC820 RIG_MAKE_MODEL(RIG_ICOM, 32)
#define RIG_MODEL_IC821 RIG_MAKE_MODEL(RIG_ICOM, 33)
#define RIG_MODEL_IC821H RIG_MAKE_MODEL(RIG_ICOM, 34)
#define RIG_MODEL_IC970 RIG_MAKE_MODEL(RIG_ICOM, 35)
#define RIG_MODEL_ICR10 RIG_MAKE_MODEL(RIG_ICOM, 36)
#define RIG_MODEL_ICR71 RIG_MAKE_MODEL(RIG_ICOM, 37)
#define RIG_MODEL_ICR72 RIG_MAKE_MODEL(RIG_ICOM, 38)
#define RIG_MODEL_ICR75 RIG_MAKE_MODEL(RIG_ICOM, 39)
#define RIG_MODEL_ICR7000 RIG_MAKE_MODEL(RIG_ICOM, 40)
#define RIG_MODEL_ICR7100 RIG_MAKE_MODEL(RIG_ICOM, 41)
#define RIG_MODEL_ICR8500 RIG_MAKE_MODEL(RIG_ICOM, 42)
#define RIG_MODEL_ICR9000 RIG_MAKE_MODEL(RIG_ICOM, 43)
#define RIG_MODEL_IC910 RIG_MAKE_MODEL(RIG_ICOM, 44)
#define RIG_MODEL_IC78 RIG_MAKE_MODEL(RIG_ICOM, 45)
#define RIG_MODEL_IC746PRO RIG_MAKE_MODEL(RIG_ICOM, 46)
#define RIG_MODEL_IC756PROII RIG_MAKE_MODEL(RIG_ICOM, 47)
#define RIG_MODEL_ICID1 RIG_MAKE_MODEL(RIG_ICOM, 54)
#define RIG_MODEL_IC703 RIG_MAKE_MODEL(RIG_ICOM, 55)
#define RIG_MODEL_IC7800 RIG_MAKE_MODEL(RIG_ICOM, 56)
#define RIG_MODEL_IC756PROIII RIG_MAKE_MODEL(RIG_ICOM, 57)
#define RIG_MODEL_ICR20 RIG_MAKE_MODEL(RIG_ICOM, 58)
#define RIG_MODEL_IC7000 RIG_MAKE_MODEL(RIG_ICOM, 60)
#define RIG_MODEL_IC7200 RIG_MAKE_MODEL(RIG_ICOM, 61)
#define RIG_MODEL_IC7700 RIG_MAKE_MODEL(RIG_ICOM, 62)
#define RIG_MODEL_IC7600 RIG_MAKE_MODEL(RIG_ICOM, 63)
#define RIG_MODEL_IC92D RIG_MAKE_MODEL(RIG_ICOM, 65)
#define RIG_MODEL_ICR9500 RIG_MAKE_MODEL(RIG_ICOM, 66)
#define RIG_MODEL_IC7410 RIG_MAKE_MODEL(RIG_ICOM, 67)
#define RIG_MODEL_IC9100 RIG_MAKE_MODEL(RIG_ICOM, 68)
#define RIG_MODEL_ICRX7 RIG_MAKE_MODEL(RIG_ICOM, 69)
#define RIG_MODEL_IC7100 RIG_MAKE_MODEL(RIG_ICOM, 70)
/* next one is 71 */

	/*
	 * Optoelectronics (CI-V)
	 */
#define RIG_MODEL_MINISCOUT RIG_MAKE_MODEL(RIG_ICOM, 48)
#define RIG_MODEL_XPLORER RIG_MAKE_MODEL(RIG_ICOM, 49)
#define RIG_MODEL_OS535 RIG_MAKE_MODEL(RIG_ICOM, 52)
#define RIG_MODEL_OS456 RIG_MAKE_MODEL(RIG_ICOM, 53)

	/*
	 * TenTec (CI-V)
	 */
#define RIG_MODEL_OMNIVI RIG_MAKE_MODEL(RIG_ICOM, 50)
#define RIG_MODEL_OMNIVIP RIG_MAKE_MODEL(RIG_ICOM, 51) /* OMNI-VI+ */
#define RIG_MODEL_PARAGON2 RIG_MAKE_MODEL(RIG_ICOM, 59)
#define RIG_MODEL_DELTAII RIG_MAKE_MODEL(RIG_ICOM, 64)

	/*
	 * Icom PCR
	 */
#define RIG_PCR 4
#define RIG_BACKEND_PCR "pcr"
#define RIG_MODEL_PCR1000 RIG_MAKE_MODEL(RIG_PCR, 1)
#define RIG_MODEL_PCR100 RIG_MAKE_MODEL(RIG_PCR, 2)
#define RIG_MODEL_PCR1500 RIG_MAKE_MODEL(RIG_PCR, 3)
#define RIG_MODEL_PCR2500 RIG_MAKE_MODEL(RIG_PCR, 4)

	/*
	 * AOR
	 */
#define RIG_AOR 5
#define RIG_BACKEND_AOR "aor"
#define RIG_MODEL_AR8200 RIG_MAKE_MODEL(RIG_AOR, 1)
#define RIG_MODEL_AR8000 RIG_MAKE_MODEL(RIG_AOR, 2)
#define RIG_MODEL_AR7030 RIG_MAKE_MODEL(RIG_AOR, 3)
#define RIG_MODEL_AR5000 RIG_MAKE_MODEL(RIG_AOR, 4)
#define RIG_MODEL_AR3030 RIG_MAKE_MODEL(RIG_AOR, 5)
#define RIG_MODEL_AR3000A RIG_MAKE_MODEL(RIG_AOR, 6)
#define RIG_MODEL_AR3000 RIG_MAKE_MODEL(RIG_AOR, 7)
#define RIG_MODEL_AR2700 RIG_MAKE_MODEL(RIG_AOR, 8)
#define RIG_MODEL_AR2500 RIG_MAKE_MODEL(RIG_AOR, 9)
#define RIG_MODEL_AR16 RIG_MAKE_MODEL(RIG_AOR, 10)
#define RIG_MODEL_SDU5500 RIG_MAKE_MODEL(RIG_AOR, 11)
#define RIG_MODEL_SDU5000 RIG_MAKE_MODEL(RIG_AOR, 12)
#define RIG_MODEL_AR8600 RIG_MAKE_MODEL(RIG_AOR, 13)
#define RIG_MODEL_AR5000A RIG_MAKE_MODEL(RIG_AOR, 14)
#define RIG_MODEL_AR7030P RIG_MAKE_MODEL(RIG_AOR, 15)
#define RIG_MODEL_SR2200 RIG_MAKE_MODEL(RIG_AOR, 16)

	/*
	 * JRC
	 */
#define RIG_JRC 6
#define RIG_BACKEND_JRC "jrc"
#define RIG_MODEL_JST145 RIG_MAKE_MODEL(RIG_JRC, 1)
#define RIG_MODEL_JST245 RIG_MAKE_MODEL(RIG_JRC, 2)
#define RIG_MODEL_CMH530 RIG_MAKE_MODEL(RIG_JRC, 3)
#define RIG_MODEL_NRD345 RIG_MAKE_MODEL(RIG_JRC, 4)
#define RIG_MODEL_NRD525 RIG_MAKE_MODEL(RIG_JRC, 5)
#define RIG_MODEL_NRD535 RIG_MAKE_MODEL(RIG_JRC, 6)
#define RIG_MODEL_NRD545 RIG_MAKE_MODEL(RIG_JRC, 7)

	/*
	 * Radio Shack
	 * Actualy, they might be either Icom or Uniden. TBC --SF
	 */
#define RIG_RADIOSHACK 7
#define RIG_BACKEND_RADIOSHACK "radioshack"
#define RIG_MODEL_RS64 RIG_MAKE_MODEL(RIG_RADIOSHACK, 1)		/* PRO-64 */
#define RIG_MODEL_RS2005 RIG_MAKE_MODEL(RIG_RADIOSHACK, 2)	/* w/ OptoElectronics OS456 Board */
#define RIG_MODEL_RS2006 RIG_MAKE_MODEL(RIG_RADIOSHACK, 3)	/* w/ OptoElectronics OS456 Board */
#define RIG_MODEL_RS2035 RIG_MAKE_MODEL(RIG_RADIOSHACK, 4)	/* w/ OptoElectronics OS435 Board */
#define RIG_MODEL_RS2042 RIG_MAKE_MODEL(RIG_RADIOSHACK, 5)	/* w/ OptoElectronics OS435 Board */
#define RIG_MODEL_RS2041 RIG_MAKE_MODEL(RIG_RADIOSHACK, 6)	/* PRO-2041 */

	/*
	 * Uniden
	 */
#define RIG_UNIDEN 8
#define RIG_BACKEND_UNIDEN "uniden"
#define RIG_MODEL_BC780 RIG_MAKE_MODEL(RIG_UNIDEN, 1)	/* Uniden BC780 - Trunk Tracker "Desktop Radio" */
#define RIG_MODEL_BC245 RIG_MAKE_MODEL(RIG_UNIDEN, 2)
#define RIG_MODEL_BC895 RIG_MAKE_MODEL(RIG_UNIDEN, 3)
#define RIG_MODEL_PRO2052 RIG_MAKE_MODEL(RIG_UNIDEN, 4)	/* Radio Shack PRO-2052 */
#define RIG_MODEL_BC235 RIG_MAKE_MODEL(RIG_UNIDEN, 5)
#define RIG_MODEL_BC250 RIG_MAKE_MODEL(RIG_UNIDEN, 6)
#define RIG_MODEL_BC785 RIG_MAKE_MODEL(RIG_UNIDEN, 7)
#define RIG_MODEL_BC786 RIG_MAKE_MODEL(RIG_UNIDEN, 8)
#define RIG_MODEL_BCT8 RIG_MAKE_MODEL(RIG_UNIDEN, 9)
#define RIG_MODEL_BCD396T RIG_MAKE_MODEL(RIG_UNIDEN, 10)
#define RIG_MODEL_BCD996T RIG_MAKE_MODEL(RIG_UNIDEN, 11)
#define RIG_MODEL_BC898 RIG_MAKE_MODEL(RIG_UNIDEN, 12)

	/*
	 * Drake
	 */
#define RIG_DRAKE 9
#define RIG_BACKEND_DRAKE "drake"
#define RIG_MODEL_DKR8 RIG_MAKE_MODEL(RIG_DRAKE, 1)
#define RIG_MODEL_DKR8A RIG_MAKE_MODEL(RIG_DRAKE, 2)
#define RIG_MODEL_DKR8B RIG_MAKE_MODEL(RIG_DRAKE, 3)

	/*
	 * Lowe
	 */
#define RIG_LOWE 10
#define RIG_BACKEND_LOWE "lowe"
#define RIG_MODEL_HF150 RIG_MAKE_MODEL(RIG_LOWE, 1)
#define RIG_MODEL_HF225 RIG_MAKE_MODEL(RIG_LOWE, 2)
#define RIG_MODEL_HF250 RIG_MAKE_MODEL(RIG_LOWE, 3)
#define RIG_MODEL_HF235 RIG_MAKE_MODEL(RIG_LOWE, 4)

	/*
	 * Racal
	 */
#define RIG_RACAL 11
#define RIG_BACKEND_RACAL "racal"
#define RIG_MODEL_RA3790 RIG_MAKE_MODEL(RIG_RACAL, 1)
#define RIG_MODEL_RA3720 RIG_MAKE_MODEL(RIG_RACAL, 2)
#define RIG_MODEL_RA6790 RIG_MAKE_MODEL(RIG_RACAL, 3)
#define RIG_MODEL_RA3710 RIG_MAKE_MODEL(RIG_RACAL, 4)
#define RIG_MODEL_RA3702 RIG_MAKE_MODEL(RIG_RACAL, 5)

	/*
	 * Watkins-Johnson
	 */
#define RIG_WJ 12
#define RIG_BACKEND_WJ "wj"
#define RIG_MODEL_HF1000 RIG_MAKE_MODEL(RIG_WJ, 1)
#define RIG_MODEL_HF1000A RIG_MAKE_MODEL(RIG_WJ, 2)
#define RIG_MODEL_WJ8711 RIG_MAKE_MODEL(RIG_WJ, 3)
#define RIG_MODEL_WJ8888 RIG_MAKE_MODEL(RIG_WJ, 4)

	/*
	 * Rohde & Schwarz
	 */
#define RIG_EK 13
#define RIG_BACKEND_EK "ek"
#define RIG_MODEL_ESM500 RIG_MAKE_MODEL(RIG_EK, 1)
#define RIG_MODEL_EK890 RIG_MAKE_MODEL(RIG_EK, 2)
#define RIG_MODEL_EK891 RIG_MAKE_MODEL(RIG_EK, 3)
#define RIG_MODEL_EK895 RIG_MAKE_MODEL(RIG_EK, 4)
#define RIG_MODEL_EK070 RIG_MAKE_MODEL(RIG_EK, 5)

	/*
	 * Skanti
	 */
#define RIG_SKANTI 14
#define RIG_BACKEND_SKANTI "skanti"
#define RIG_MODEL_TRP7000 RIG_MAKE_MODEL(RIG_SKANTI, 1)
#define RIG_MODEL_TRP8000 RIG_MAKE_MODEL(RIG_SKANTI, 2)
#define RIG_MODEL_TRP9000 RIG_MAKE_MODEL(RIG_SKANTI, 3)
#define RIG_MODEL_TRP8255 RIG_MAKE_MODEL(RIG_SKANTI, 4)

	/*
	 * WiNRADiO/LinRADiO
	 */
#define RIG_WINRADIO 15
#define RIG_BACKEND_WINRADIO "winradio"
#define RIG_MODEL_WR1000 RIG_MAKE_MODEL(RIG_WINRADIO, 1)
#define RIG_MODEL_WR1500 RIG_MAKE_MODEL(RIG_WINRADIO, 2)
#define RIG_MODEL_WR1550 RIG_MAKE_MODEL(RIG_WINRADIO, 3)
#define RIG_MODEL_WR3100 RIG_MAKE_MODEL(RIG_WINRADIO, 4)
#define RIG_MODEL_WR3150 RIG_MAKE_MODEL(RIG_WINRADIO, 5)
#define RIG_MODEL_WR3500 RIG_MAKE_MODEL(RIG_WINRADIO, 6)
#define RIG_MODEL_WR3700 RIG_MAKE_MODEL(RIG_WINRADIO, 7)
#define RIG_MODEL_G303 RIG_MAKE_MODEL(RIG_WINRADIO, 8)
#define RIG_MODEL_G313 RIG_MAKE_MODEL(RIG_WINRADIO, 9)
#define RIG_MODEL_G305 RIG_MAKE_MODEL(RIG_WINRADIO, 10)
#define RIG_MODEL_G315 RIG_MAKE_MODEL(RIG_WINRADIO, 11)

	/*
	 * Ten Tec
	 */
#define RIG_TENTEC 16
#define RIG_BACKEND_TENTEC "tentec"
#define RIG_MODEL_TT550 RIG_MAKE_MODEL(RIG_TENTEC, 1)	/* Pegasus */
#define RIG_MODEL_TT538 RIG_MAKE_MODEL(RIG_TENTEC, 2)	/* Jupiter */
#define RIG_MODEL_RX320 RIG_MAKE_MODEL(RIG_TENTEC, 3)
#define RIG_MODEL_RX340 RIG_MAKE_MODEL(RIG_TENTEC, 4)
#define RIG_MODEL_RX350 RIG_MAKE_MODEL(RIG_TENTEC, 5)
#define RIG_MODEL_TT526 RIG_MAKE_MODEL(RIG_TENTEC, 6)	/* 6N2 */
#define RIG_MODEL_TT516 RIG_MAKE_MODEL(RIG_TENTEC, 7)	/* Argonaut V */
#define RIG_MODEL_TT565 RIG_MAKE_MODEL(RIG_TENTEC, 8)	/* Orion */
#define RIG_MODEL_TT585 RIG_MAKE_MODEL(RIG_TENTEC, 9)	/* Paragon */
#define RIG_MODEL_TT588 RIG_MAKE_MODEL(RIG_TENTEC, 11)	/* Omni-VII */
#define RIG_MODEL_RX331 RIG_MAKE_MODEL(RIG_TENTEC, 12)
#define RIG_MODEL_TT599 RIG_MAKE_MODEL(RIG_TENTEC, 13)  /* Eagle */

	/*
	 * Alinco
	 */
#define RIG_ALINCO 17
#define RIG_BACKEND_ALINCO "alinco"
#define RIG_MODEL_DX77 RIG_MAKE_MODEL(RIG_ALINCO, 1)

	/*
	 * Kachina
	 */
#define RIG_KACHINA 18
#define RIG_BACKEND_KACHINA "kachina"
#define RIG_MODEL_505DSP RIG_MAKE_MODEL(RIG_KACHINA, 1)


	/*
	 * Gnuradio backend
	 */
#define RIG_GNURADIO 20
#define RIG_BACKEND_GNURADIO "gnuradio"
#define RIG_MODEL_GNURADIO RIG_MAKE_MODEL(RIG_GNURADIO, 1) /* dev model, Chirp source */
#define RIG_MODEL_MC4020 RIG_MAKE_MODEL(RIG_GNURADIO, 2)	/* MC4020 */
#define RIG_MODEL_GRAUDIO RIG_MAKE_MODEL(RIG_GNURADIO, 3)	/* Sound card source */
#define RIG_MODEL_GRAUDIOIQ RIG_MAKE_MODEL(RIG_GNURADIO, 4)	/* I&Q stereo sound card source */
#define RIG_MODEL_USRP_G RIG_MAKE_MODEL(RIG_GNURADIO, 5)	/* Universal Software Radio Peripheral */

	/*
	 * Microtune tuners
	 */
#define RIG_MICROTUNE 21
#define RIG_BACKEND_MICROTUNE "microtune"
#define RIG_MODEL_MICROTUNE_4937 RIG_MAKE_MODEL(RIG_MICROTUNE, 1)	/* eval board */
#define RIG_MODEL_MICROTUNE_4702 RIG_MAKE_MODEL(RIG_MICROTUNE, 2)	/* Alan's */
#define RIG_MODEL_MICROTUNE_4707 RIG_MAKE_MODEL(RIG_MICROTUNE, 3)

	/*
	 * TAPR
	 */
#define RIG_TAPR 22
#define RIG_BACKEND_TAPR "tapr"
#define RIG_MODEL_DSP10 RIG_MAKE_MODEL(RIG_TAPR, 1)

	/*
	 * Flex-radio
	 */
#define RIG_FLEXRADIO 23
#define RIG_BACKEND_FLEXRADIO "flexradio"
#define RIG_MODEL_SDR1000 RIG_MAKE_MODEL(RIG_FLEXRADIO, 1)
#define RIG_MODEL_SDR1000RFE RIG_MAKE_MODEL(RIG_FLEXRADIO, 2)
#define RIG_MODEL_DTTSP RIG_MAKE_MODEL(RIG_FLEXRADIO, 3)
#define RIG_MODEL_DTTSP_UDP RIG_MAKE_MODEL(RIG_FLEXRADIO, 4)


	/*
	 * VEB Funkwerk K�penick RFT
	 */
#define RIG_RFT 24
#define RIG_BACKEND_RFT "rft"
#define RIG_MODEL_EKD500 RIG_MAKE_MODEL(RIG_RFT, 1)

	/*
	 * Various kits
	 */
#define RIG_KIT 25
#define RIG_BACKEND_KIT "kit"
#define RIG_MODEL_ELEKTOR304 RIG_MAKE_MODEL(RIG_KIT, 1)
#define RIG_MODEL_DRT1 RIG_MAKE_MODEL(RIG_KIT, 2)
#define RIG_MODEL_DWT RIG_MAKE_MODEL(RIG_KIT, 3)
#define RIG_MODEL_USRP0 RIG_MAKE_MODEL(RIG_KIT, 4)	/* prototype */
#define RIG_MODEL_USRP RIG_MAKE_MODEL(RIG_KIT, 5)
#define RIG_MODEL_DDS60 RIG_MAKE_MODEL(RIG_KIT, 6)
#define RIG_MODEL_ELEKTOR507 RIG_MAKE_MODEL(RIG_KIT, 7)	/* Elektor SDR USB */
#define RIG_MODEL_MINIVNA RIG_MAKE_MODEL(RIG_KIT, 8)
#define RIG_MODEL_SI570AVRUSB RIG_MAKE_MODEL(RIG_KIT, 9) /* SoftRock Si570 AVR */
#define RIG_MODEL_PMSDR RIG_MAKE_MODEL(RIG_KIT, 10)
#define RIG_MODEL_SI570PICUSB RIG_MAKE_MODEL(RIG_KIT, 11) /* SoftRock Si570 PIC */
#define RIG_MODEL_FIFISDR RIG_MAKE_MODEL(RIG_KIT, 12) /* FiFi-SDR USB */
#define RIG_MODEL_FUNCUBEDONGLE RIG_MAKE_MODEL(RIG_KIT, 13)	/* FunCUBE Dongle */
#define RIG_MODEL_HIQSDR RIG_MAKE_MODEL(RIG_KIT, 14)	/* HiQSDR */
#define RIG_MODEL_FASDR RIG_MAKE_MODEL(RIG_KIT,15) /* Funkamateur Sdr */
#define RIG_MODEL_SI570PEABERRY1 RIG_MAKE_MODEL(RIG_KIT, 16) /* Peaberry V1 */
#define RIG_MODEL_SI570PEABERRY2 RIG_MAKE_MODEL(RIG_KIT, 17) /* Peaberry V2 */
#define RIG_MODEL_FUNCUBEDONGLEPLUS RIG_MAKE_MODEL(RIG_KIT, 18)	/* FunCUBE Dongle Pro+ */

	/*
	 * SW/FM/TV tuner cards supported by Video4Linux,*BSD, ..
	 */
#define RIG_TUNER 26
#define RIG_BACKEND_TUNER "tuner"
#define RIG_MODEL_V4L RIG_MAKE_MODEL(RIG_TUNER, 1)
#define RIG_MODEL_V4L2 RIG_MAKE_MODEL(RIG_TUNER, 2)

	/*
	 * Rohde&Schwarz
	 */
#define RIG_RS 27
#define RIG_BACKEND_RS "rs"
#define RIG_MODEL_ESMC RIG_MAKE_MODEL(RIG_RS, 1)
#define RIG_MODEL_EB200 RIG_MAKE_MODEL(RIG_RS, 2)

	/*
	 * Phillips/Simoco PRM80
	 */
#define RIG_PRM80 28
#define RIG_BACKEND_PRM80 "prm80"
#define RIG_MODEL_PRM8060 RIG_MAKE_MODEL(RIG_PRM80, 1)
#define RIG_MODEL_PRM8070 RIG_MAKE_MODEL(RIG_PRM80, 2)

        /*
         * ADAT by HB9CBU
         *
         * ADDED: frgo (DG1SBG), 2012-01-01
         */
#define RIG_ADAT 29
#define RIG_BACKEND_ADAT "adat"
#define RIG_MODEL_ADT_200A RIG_MAKE_MODEL(RIG_ADAT, 1)

	/*
	 * TODO:
		RIG_MODEL_KWZ30,	KNEISNER +DOERING
		RIG_MODEL_E1800,	DASA-Telefunken
		etc.
	*/

/*! \typedef typedef int rig_model_t
    \brief Convenience type definition for rig model.
*/
typedef int rig_model_t;

/*
 * struct rig_backend_list {
 *		rig_model_t model;
 *		const char *backend;
 * } rig_backend_list[] = RIG_LIST;
 *
 * TODO:
 *
	{ RIG_RADIOSHACK, RIG_BACKEND_RADIOSHACK }, \
 */

#endif /* _RIGLIST_H */
