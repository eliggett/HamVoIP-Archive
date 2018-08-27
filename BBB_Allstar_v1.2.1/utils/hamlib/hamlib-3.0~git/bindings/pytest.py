#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

sys.path.append ('.')
sys.path.append ('.libs')

import Hamlib

def StartUp ():
    print "Python", sys.version[:5], "test,", Hamlib.cvar.hamlib_version, "\n"

    #Hamlib.rig_set_debug (Hamlib.RIG_DEBUG_TRACE)
    Hamlib.rig_set_debug (Hamlib.RIG_DEBUG_NONE)

    # Init RIG_MODEL_DUMMY
    my_rig = Hamlib.Rig (Hamlib.RIG_MODEL_DUMMY)
    my_rig.set_conf ("rig_pathname","/dev/Rig")
    my_rig.set_conf ("retry","5")

    my_rig.open ()

    # 1073741944 is token value for "itu_region"
    # but using get_conf is much more convenient
    region = my_rig.get_conf(1073741944)
    rpath = my_rig.get_conf("rig_pathname")
    retry = my_rig.get_conf("retry")
    print "status(str):\t\t",Hamlib.rigerror(my_rig.error_status)
    print "get_conf:\t\tpath = %s, retry = %s, ITU region = %s" \
        % (rpath, retry, region)

    my_rig.set_freq (Hamlib.RIG_VFO_B, 5700000000)
    my_rig.set_vfo (Hamlib.RIG_VFO_B)
    print "freq:\t\t\t",my_rig.get_freq()
    my_rig.set_freq (Hamlib.RIG_VFO_A, 145550000)
    #my_rig.set_vfo ("VFOA")

    (mode, width) = my_rig.get_mode()
    print "mode:\t\t\t",Hamlib.rig_strrmode(mode),"\nbandwidth:\t\t",width
    my_rig.set_mode(Hamlib.RIG_MODE_CW)
    (mode, width) = my_rig.get_mode()
    print "mode:\t\t\t",Hamlib.rig_strrmode(mode),"\nbandwidth:\t\t",width

    print "ITU_region:\t\t",my_rig.state.itu_region
    print "Backend copyright:\t",my_rig.caps.copyright

    print "Model:\t\t\t",my_rig.caps.model_name
    print "Manufacturer:\t\t",my_rig.caps.mfg_name
    print "Backend version:\t",my_rig.caps.version
    print "Backend license:\t",my_rig.caps.copyright
    print "Rig info:\t\t", my_rig.get_info()

    my_rig.set_level ("VOX",  1)
    print "VOX level:\t\t",my_rig.get_level_i("VOX")
    my_rig.set_level (Hamlib.RIG_LEVEL_VOX, 5)
    print "VOX level:\t\t", my_rig.get_level_i(Hamlib.RIG_LEVEL_VOX)

    print "strength:\t\t", my_rig.get_level_i(Hamlib.RIG_LEVEL_STRENGTH)
    print "status:\t\t\t",my_rig.error_status
    print "status(str):\t\t",Hamlib.rigerror(my_rig.error_status)

    chan = Hamlib.channel(Hamlib.RIG_VFO_B)

    my_rig.get_channel(chan)
    print "get_channel status:\t",my_rig.error_status

    print "VFO:\t\t\t",Hamlib.rig_strvfo(chan.vfo),", ",chan.freq

    print "\nSending Morse, '73'"
    my_rig.send_morse(Hamlib.RIG_VFO_A, "73")

    my_rig.close ()

    print "\nSome static functions:"

    err, lon1, lat1 = Hamlib.locator2longlat("IN98XC")
    err, lon2, lat2 = Hamlib.locator2longlat("DM33DX")
    err, loc1 = Hamlib.longlat2locator(lon1, lat1, 3)
    err, loc2 = Hamlib.longlat2locator(lon2, lat2, 3)
    print "Loc1:\t\tIN98XC -> %9.4f, %9.4f -> %s" % (lon1, lat1, loc1)
    print "Loc2:\t\tDM33DX -> %9.4f, %9.4f -> %s" % (lon2, lat2, loc2)

    err, dist, az = Hamlib.qrb(lon1, lat1, lon2, lat2)
    longpath = Hamlib.distance_long_path(dist)
    print "Distance:\t%.3f km, azimuth %.2f, long path:\t%.3f km" \
        % (dist, az, longpath)

    # dec2dms expects values from 180 to -180
    # sw is 1 when deg is negative (west or south) as 0 cannot be signed
    err, deg1, mins1, sec1, sw1 = Hamlib.dec2dms(lon1)
    err, deg2, mins2, sec2, sw2 = Hamlib.dec2dms(lat1)

    lon3 = Hamlib.dms2dec(deg1, mins1, sec1, sw1)
    lat3 = Hamlib.dms2dec(deg2, mins2, sec2, sw2)

    print 'Longitude:\t%4.4f, %4d° %2d\' %2d" %1s\trecoded: %9.4f' \
        % (lon1, deg1, mins1, sec1, ('W' if sw1 else 'E'), lon3)

    print 'Latitude:\t%4.4f, %4d° %2d\' %2d" %1s\trecoded: %9.4f' \
        % (lat1, deg2, mins2, sec2, ('S' if sw2 else 'N'), lat3)

if __name__ == '__main__':
    StartUp ()
