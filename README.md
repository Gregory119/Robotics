# Robotics
This code is used for the camera based robot that I have created.

# CROSS-COMPILING
References: 
- [1] https://www.raspberrypi.org/documentation/linux/kernel/building.md
- [2] http://www.linfo.org/path_env_var.html
- [3] https://help.ubuntu.com/community/EnvironmentVariables
- [4] http://www.fabriziodini.eu/posts/cross_compile_tutorial/    (good explanation of overall process and concepts)
- [5] https://wiki.qt.io/RaspberryPi_Beginners_Guide  (helpful tips - eg. mounting pi image for sysroot directory)
- [6] https://gcc.gnu.org/onlinedocs/gcc-2.95.2/gcc_2.html
- core guide: http://hackaday.com/2016/02/03/code-craft-cross-compiling-for-the-raspberry-pi/
- arm optimization flags: 
  - https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
  - http://raspberrypi.stackexchange.com/questions/2046/which-cpu-flags-are-suitable-for-gcc-on-raspberry-pi

on the host:
# install compiling utilities such as g++, gcc etc
apt-get install build-essential

# create directory structure
 - $ mkdir /rasppi_xcompile
 - $ cd /rasppi_xcompile => all raspberry pi cross compile files
 - $ mkdir extra_libs_srcs => for source files of additional libraries
 - $ mkdir extra_libs_install => for install files of additional libraries
 - $ mkdir sd_card_images => for distro sd card images 
 - $ mkdir sysroot => to be used for mounting the sd card images

# install arm compilation tools
- download the raspberry pi tool chain [1]
  - $ git clone https://github.com/raspberrypi/tools
  
- add toolchain bin directory to the PATH environment variable [2] to allow tool execution from any directory:
  - create a file /etc/profile.d/bash.rc for system-wide variables [3]
  - add the following lines:
  export PITOOLCHAIN=/rasppi_xcompile/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin
  export PATH=$PATH:$PITOOLCHAIN

# mount sd_card image to sysroot [5]
- download the image to the sd_card_images folder (I used 2016-05-27-raspbian-jessie-lite)
- unzip the file
- check the offset for mounting
  - $ fdisk -l /path/to/imagefile
  - multiple the start value of the second listed image by the sector size (for the above image 137216*512 = 70254592)
- mount the image
  - $ mount -o loop,offset=70254592 2016-05-27-raspbian-jessie-lite.img /rasppi_xcompile/sysroot/

# get extra libraries/binaries
Either use precompiled files with instructions in Robotics/arm_pkgs
OR compile on the pi and copy across,
OR cross compile [4], [6],
OR apt-get install on the Pi and copy across (if other options fail)

!!TO DO: create systemd startup script for servoblaster
!!TO DO: add libcurl for cross compile and basic testing
# 








# install arch independent gdb client
apt-get install gdb-multiarch

on the Pi:
# gdb to be connected to by the client gdb
apt-get install gdbserver

# set up a nfs server on the pi to be linked
# to by the 

#check out
google search: "gcc arm cross compile configure library path"
http://stackoverflow.com/questions/7476625/set-global-gcc-default-search-paths