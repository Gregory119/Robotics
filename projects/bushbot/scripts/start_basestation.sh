#!/bin/bash

#this script currently does not handle the case when there is usb controller read error
#possible future solutions:
# 1) restart binaries if there are no events after a certain period (eg. 60s)

dir=/Bushbot/
check_time_sec=5 
#uncomment one of the following variables to run as a robot or basestation
binary="basestation"
#binary="robot"

while true; do
    if [ "$binary"=="basestation" ]; then
	if [ -z $(pidof xboxdrv) ]; then
	    echo "Xbox controller driver not running."
	    echo "Stopping binary $(binary)."
	    pkill $binary
	    echo "Starting xbox controller driver."
	    xboxdrv -d -s --quiet -l 2 > /dev/null 2>&1 #do not care about output
	    cd $dir
	    echo "Starting binary $(binary)."
	    ./$(binary) > /dev/null 2>&1
	elif [ -z $(pidof $binary) ]; then
	    echo "Binary $(binary) not running. Restarting.."
	    cd $dir
	    ./$(binary) > /dev/null 2>&1
	fi
    elif [ "$binary"=="robot" ]; then
	if [ -z $(pidof $binary) ]; then
	    echo "Binary $(binary) not running. Restarting.."
	    cd $dir
	    ./$(binary) > /dev/null 2>&1
	fi
    fi
    sleep $check_time_sec
done

