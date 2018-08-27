#!/bin/sh
# the next line restarts using tclsh \
exec tclsh "$0" "$@"

# Edit the path below to reflect installed Hamlib extension
lappend ::auto_path /usr/local/lib/tcl/Hamlib

## Brute force loading
#load "/usr/local/lib/tcltk/Hamlib/hamlibtcl.so" Hamlib

## Preferred package loading
package require hamlib

set tclver [info tclversion]
puts "Tcl $tclver test, $hamlib_version\n"

#rig_set_debug $RIG_DEBUG_TRACE
rig_set_debug $RIG_DEBUG_NONE

# Init RIG_MODEL_DUMMY
Rig my_rig $RIG_MODEL_DUMMY

my_rig open
my_rig set_freq $RIG_VFO_A 145550000

puts "status:\t\t[my_rig cget -error_status]"

# get_mode returns a tuple
set moderes [my_rig get_mode]
set mode [rig_strrmode [lindex $moderes 0]]
puts "mode:\t\t$mode\nbandwidth:\t[lindex $moderes 1]Hz"

set state [my_rig cget -state]
puts "ITU_region:\t[$state cget -itu_region]"

# The following works well also
# puts ITU_region:[[my_rig cget -state] cget -itu_region]

puts "getinfo:\t[my_rig get_info]"

my_rig set_level "VOX"  1
puts "status:\t\t[my_rig cget -error_status]"
puts "VOX level:\t[my_rig get_level_i 'VOX']"
puts "status:\t\t[my_rig cget -error_status]"
my_rig set_level $RIG_LEVEL_VOX 5
puts "status:\t\t[my_rig cget -error_status]"
puts "VOX level:\t[my_rig get_level_i $RIG_LEVEL_VOX]"
puts "status:\t\t[my_rig cget -error_status]"

puts "strength:\t[my_rig get_level_i $RIG_LEVEL_STRENGTH]"
puts "status:\t\t[my_rig cget -error_status]"
puts "status(str):\t[rigerror [my_rig cget -error_status]]"

puts "\nSending Morse, '73'"
my_rig send_morse $RIG_VFO_A "73"

my_rig close
#my_rig cleanup


exit 0
