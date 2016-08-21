#include the list of libraries
#if library is used then add object files, inlude directories, and other required LIBS
BASE_DIR = ..
#source files in current directory
SRC = main.cpp coms_serialjoystick.cpp
OBJ = $(SRC:.cpp=.o)
# add libraries
OBJ += $(BASE_DIR)/joystick/js_joystick.o $(BASE_DIR)/kern/kn_basic.o
CXX = g++
# use this to include header files in different directories
INCLUDES = -I$(BASE_DIR)/joystick -I$(BASE_DIR)/kern
LIBS = -lpthread -lwiringPi
# use -std=c++11 => c++11, -Wall => all warnings enabled
CXXFLAGS = -std=c++11 -Wall
NAME = testprog

makelibs:
#make prerequisite libraries
	$(MAKE) -C $(BASE_DIR)/joystick
	$(MAKE) -C $(BASE_DIR)/kern
	$(MAKE) thisdir

thisdir: $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(OBJ) -o $(NAME)

include ../makedef/makefile.rules