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

# compile extra libraries 
- servo blaster [4], [6], and respective Makefile
  - $ cd extra_libs_scrs
  - $ git clone https://github.com/richardghirst/PiBits.git
  - $ cd PiBits/ServoBlaster/user/
  - edit Makefile line which has "gcc -Wall -g -O2 -o servod servod.c mailbox.c -lm" to:
  arm-linux-gnueabihf-gcc -Wall -g -O2 --sysroot=/rasppi_xcompile/sysroot -I./include -I/usr/local/include -o servod servod.c mailbox.c -lm
  - copy the servod binary to the desired location on the mounted image directory
  - add systemd start-up script for servoblaster on the sd card image once booted
- wiringPi
  - compile this on the pi 
  - $ git clone git://git.drogon.net/wiringPi
  - $ make
  - $ make install
  - copy the library and header files to /rasppi_xcompile/extra_libs_install/

!!TO DO: create systemd startup script for servoblaster
!!TO DO: copy the wiringPi header and library files to the host machine
!!TO DO: add cross compile compatibility in project makefiles
!!TO DO: cross compile project makefiles
!!TO DO: test on the pi
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