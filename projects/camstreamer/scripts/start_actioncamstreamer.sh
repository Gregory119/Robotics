#!/bin/bash

set -e # exit shell on a simple command failure

project_dir=/.actioncamstreamer
binary="actioncamstreamer"
expand_file=fs_expanded

# First expand the file system, if it has not been done already, and then reboot.
cd $project_dir
if [ ! -e "$expand_file" ]
then
    touch $expand_file
    raspi-config --expand-rootfs
    shutdown -r now
else
    while true; do
	if [ -z $(pidof $binary) ]; then
	    #echo "Starting $binary."
	    ./$binary > /dev/null 2>&1 &
	fi
	if [ -z $(pidof "omxplayer") ]; then
	    #echo "Starting omxplayer"
	    omxplayer --live udp://:8554 > /dev/null 2>&1
	    # Run in the foreground because omxplayer has a timeout when no data has been received
	fi
    done
fi
