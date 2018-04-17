#!/bin/bash

project_dir=/actioncamstreamer
binary="actioncamstreamer"

while true; do
    if [ -z $(pidof $binary) ]; then
	cd $project_dir
	echo "Starting $binary."
	./$binary > /dev/null 2>&1 &
    fi
    if [ -z $(pidof "omxplayer") ]; then
	echo "Starting omxplayer"
	omxplayer --live udp://:8554 > /dev/null 2>&1
	# Run in the foreground because omxplayer has a timeout when no data has been received
    fi
done

