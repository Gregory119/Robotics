# Robotics
This code is used for the camera based robot that I have created.

# CROSS-COMPILING
References: 
- core guide: http://hackaday.com/2016/02/03/code-craft-cross-compiling-for-the-raspberry-pi/
- arm optimization flags: 
  - https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
  - http://raspberrypi.stackexchange.com/questions/2046/which-cpu-flags-are-suitable-for-gcc-on-raspberry-pi

on the host:
# install compiling utilities such as g++, gcc etc
apt-get install build-essential
# install arm compilation tools
apt-get install gcc-arm-linux-gnueabihf
apt-get install g++-arm-linux-gnueabihf
# install arch independent gdb client
apt-get install gdb-multiarch

on the Pi:
# gdb to be coonected to by the client gdb
apt-get install gdbserver

# set up a nfs server on the pi to be linked
# to by the 

#check out
google search: "gcc arm cross compile configure library path"
http://stackoverflow.com/questions/7476625/set-global-gcc-default-search-paths