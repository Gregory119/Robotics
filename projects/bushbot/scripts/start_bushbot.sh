#!/bin/bash

project_dir=/Bushbot
startscript_dir=/usr/startscipts
check_time_sec=5 
#uncomment one of the following variables to run as a robot or basestation
binary="basestation"
#binary="robot"

while true; do
    if [ "$binary" == "basestation" ]; then
	if [ -z $(pidof $binary) ]; then
	    cd $project_dir
	    echo "Starting $binary."
	    ./$binary > /dev/null 2>&1
	fi
    elif [ "$binary" == "robot" ]; then
	if [ -z $(pidof $binary) ]; then
	    if [ -z $(pidof servod) ]; then
		./$startscript_dir/start_servoblaster.sh > /dev/null 2>&1
	    fi
	    echo "Starting $binary."
	    cd $project_dir
	    ./$binary > /dev/null 2>&1
	fi
    fi
    sleep $check_time_sec
done

