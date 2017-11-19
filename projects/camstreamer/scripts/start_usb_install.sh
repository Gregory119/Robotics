#!/bin/bash
# This script is called to mount the usb drive and start the install script located on the usb drive.
install_folder=install_files
mount_dir=/media/usb_drive/

# MAKE SURE THAT THE DEVNAME ENVIRONMENT VARIABLE IS AVAILABLE WHEN BEING RUN WITH A SYSTEMD SERVICE, WHICH IS RUN FROM A UDEV RULE (PRINT TO FILE)
# mount the drive
mkdir $mount_dir
mount $DEVNAME $mount_dir

# run the install script on the usb drive
$mount_dir$install_folder/usb_install.sh

# unmount when complete
umount $mount_dir
