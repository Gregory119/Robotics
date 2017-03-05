#!/bin/bash

bin_dir=/usr/local/bin
check_time_sec=5 
binary="servod"

while true; do
    if [ -z $(pidof $binary) ]; then
	cd $bin_dir
	echo "Starting $binary."
	./$binary > /dev/null 2>&1
    fi
    sleep $check_time_sec
done
