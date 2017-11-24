#!/bin/bash
# This script will be on the usb drive, and will be run when the drive is plugged in.

set -e # exit shell on a simple command failure
install_dir=$1
led_dir=/sys/class/leds/led0

# First kill the firmware and start script
# This is killed first because it can control the LED
systemctl kill --kill-who=all start_actioncamstreamer.service

# start LED flash
echo "timer" >> $led_dir/trigger
echo 250 >> $led_dir/delay_off
echo 250 >> $led_dir/delay_on

# start installing the files
# copy the new firmware
cd $install_dir
cp firmware/* .
chmod 777 actioncamstreamer

sleep 5 # so that the user has time to see that the install has triggered

# restart the firmware start script service
systemctl restart start_actioncamstreamer.service

# stop LED flash
echo "mmc0" >> $led_dir/trigger
