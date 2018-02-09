#!/bin/bash
# This script is called to mount the usb drive and start the install script located on the usb drive.

set -e # exit shell on a simple command failure
install_dir=$1 # eg. /actioncamstreamer
dev_name=$2 # eg. /dev/sdd
partition_num=1
mount_dir=/mount/usb_drive/

# mount the drive
mkdir -p $mount_dir
mount $dev_name$partition_num $mount_dir

# install 
cd $install_dir
cp $mount_dir/install.tgz . # copy the compressed firmware install files
tar xzf install.tgz # extract the files
chmod 777 install.sh # give file permissions CHANGE THESE TO SOMETHING MORE APPROPRIATE
./install.sh $install_dir $mount_dir # run the script of the install files

# unmount when complete
umount $mount_dir
