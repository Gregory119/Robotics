#!/bin/bash

# This script will be on the usb drive, and will be run when the drive is plugged in.
led_dir=/sys/class/leds/led0

# start LED flash
echo "timer" >> $led_dir/trigger
echo 250 >> $led_dir/delay_off
echo 250 >> $led_dir/delay_on

# temporary sleep for testing
sleep 10

# stop LED flash
echo "mmc0" >> $led_dir/trigger
