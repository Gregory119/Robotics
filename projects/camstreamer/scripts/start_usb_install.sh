#!/bin/bash
# This script is called to mount the usb drive and start the install script located on the usb drive.

set -e # exit shell on a simple command failure
install_dir=$1 # eg. /actioncamstreamer
dev_name=$2 # eg. /dev/sdd
partition_num=1
mount_dir=/mount/usb_drive/
usb_firmware_dir=$mount_dir/GoPie/
binary_name=actioncamstreamer

# start LED flash
led_dir=/sys/class/leds/led0
echo "timer" >> $led_dir/trigger
echo 250 >> $led_dir/delay_off
echo 250 >> $led_dir/delay_on

# mount the drive
mkdir -p $mount_dir
mount $dev_name$partition_num $mount_dir

# install
# run install here for safety, instead of running a script on the usb
cd $install_dir
cp $usb_firmware_dir/firmware.tgz . # copy the compressed firmware install files
tar xzf firmware.tgz # extract the files

# First kill the firmware and start script
# This is killed first because it can control the LED
systemctl kill --kill-who=all start_actioncamstreamer.service

# copy the new firmware
cp firmware/* .
chmod +x $binary_name

# copy recent logs to the usb
config_filename="GoPie_userconfig.txt"
log_filename="GoPie_logs.txt"

# copy log file if it exists
if [ -f "$log_filename" ]
then
  cp $log_filename $usb_firmware_dir
fi

# copy config file if it exists
if [ -f "$usb_firmware_dir/$config_filename" ]
then
  cp $usb_firmware_dir/$config_filename .
fi

sleep 3 # so that the user has time to see that the install has triggered

# Restart the firmware start script service. The firmware will then take control of the led.
systemctl restart start_actioncamstreamer.service

# unmount when complete
umount $mount_dir
