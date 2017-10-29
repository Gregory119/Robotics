#!/bin/bash

project_dir=/camstreamer
check_time_sec=1
binary="camstreamer"

while true; do
    if [ -z $(pidof $binary) ]; then
	cd $project_dir
	echo "Starting $binary."
	./$binary > /dev/null 2>&1 &
    fi
    if [ -z $(pidof "omxplayer") ]; then
	echo "Starting omxplayer"
	omxplayer --live -o hdmi udp://:8554 > /dev/null 2>&1
	# omxplayer has a timeout when not receiving data

    fi

    sleep $check_time_sec
done

