#!/bin/bash

project_dir=/camstreamer
startscript_dir=/usr/startscipts
check_time_sec=5 
#uncomment one of the following variables to run as a robot or basestation
binary="camstreamer"

while true; do
    if [ -z $(pidof $binary) ]; then
	cd $project_dir
	echo "Starting $binary."
	/$binary > /dev/null 2>&1
    fi
    sleep $check_time_sec
done

