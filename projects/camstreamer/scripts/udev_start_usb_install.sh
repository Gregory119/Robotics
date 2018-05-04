#!/bin/bash
# This script calls the usb install start script and sends it to the background to process.
set -e # exit shell on a simple command failure
dir=/.actioncamstreamer
$dir/start_usb_install.sh $dir $DEVNAME &
