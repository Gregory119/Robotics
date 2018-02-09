#!/bin/bash
# This script will be in the files copied from the usb drive, and will be run to install the usb files.

set -e # exit shell on a simple command failure
install_dir=$1
mount_dir=$2
config_filename="settings.txt"
led_dir=/sys/class/leds/led0

# First kill the firmware and start script
# This is killed first because it can control the LED
systemctl kill --kill-who=all start_actioncamstreamer.service

# start LED flash
echo "timer" >> $led_dir/trigger
echo 250 >> $led_dir/delay_off
echo 250 >> $led_dir/delay_on

# start installing the files
# copy config file if it exists
cd $mount_dir
if [-f "$config_filename"]
then
  cp $config_filename $install_dir  
fi

# copy the new firmware
cd $install_dir
cp firmware/* .
chmod 777 actioncamstreamer

sleep 3 # so that the user has time to see that the install has triggered

# restart the firmware start script service
systemctl restart start_actioncamstreamer.service

# stop LED flash
echo "mmc0" >> $led_dir/trigger
