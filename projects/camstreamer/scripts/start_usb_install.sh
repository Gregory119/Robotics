#!/bin/bash
# This script is called to mount the usb drive and start the install script located on the usb drive.

set -e # exit shell on a simple command failure
install_dir=$1 # eg. /actioncamstreamer
dev_name=/dev/$2 # eg. /dev/sdd1
mount_dir=/mount/usb_drive/
usb_firmware_dir=$mount_dir/GoPie/
binary_name=actioncamstreamer

# start LED flash
led_dir=/sys/class/leds/led0
echo "timer" >> $led_dir/trigger
echo 150 >> $led_dir/delay_off
echo 150 >> $led_dir/delay_on

# mount the drive
mkdir -p $mount_dir
mount $dev_name $mount_dir

cd $install_dir

# First kill the firmware and start script
# This is killed first because it can control the LED
systemctl kill --kill-who=all start_actioncamstreamer.service

# install firmware if it exists on the usb
# run install here for safety, instead of running a script on the usb
if [ -e "$usb_firmware_dir/firmware.tgz" ]
then
    cp $usb_firmware_dir/firmware.tgz . # copy the compressed firmware install files
    tar xzf firmware.tgz # extract the files

    # copy the new firmware
    cp firmware/* .
    chmod +x $binary_name
fi

# copy log file if it exists
if [ -e *.log ]
then
    cp *.log $usb_firmware_dir
    if [ -d backup_logs ]
    then
	cp -r backup_logs $usb_firmware_dir
    fi
fi

# copy config file if it exists
if [ -e "$usb_firmware_dir/GoPie_userconfig.txt" ]
then
  cp $usb_firmware_dir/$config_filename .
fi

sleep 3 # so that the user has time to see that the install has triggered
echo "0" >> $led_dir/brightness # turn off

# Restart the firmware start script service. The firmware will then take control of the led.
systemctl restart start_actioncamstreamer.service

# unmount when complete
umount $mount_dir
