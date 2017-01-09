#!/bin/bash

#this script currently does not handle the case when there is usb controller read error
#possible future solutions:
# 1) restart binaries if there are no events after a certain period (eg. 60s)

project_dir=/Bushbot
check_time_sec=5 
#uncomment one of the following variables to run as a robot or basestation
binary="basestation"
#binary="robot"

while true; do
    if [ "$binary" == "basestation" ]; then
	if [ -z $(pidof $binary) ]; then
	    #if [ -z $(pidof xboxdrv) ]; then
		#echo "Starting xbox controller driver."
		#xboxdrv -d -s --quiet -l 2 > /dev/null 2>&1 & #do not care about output
	    #fi
	    cd $project_dir
	    echo "Starting $binary."
	    ./$binary > /dev/null 2>&1
	fi
    elif [ "$binary" == "robot" ]; then
	if [ -z $(pidof $binary) ]; then
	    echo "Starting $binary."
	    cd $project_dir
	    ./$binary > /dev/null 2>&1
	fi
    fi
    sleep $check_time_sec
done

