#!/bin/bash
# This script calls the usb install start script and sends it to the background to process.
dir=/actioncamstreamer
$dir/start_usb_install.sh $dir $DEVNAME &
