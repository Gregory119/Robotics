SRC = main.cpp js_joystick.cpp js_common.cpp
OBJ = $(SRC:.cpp=.o)
CXX = g++
# use this to include header files in different directories
INCLUDES = 
LIBS = -lpthread
# use -std=c++11 => c++11, -Wall => all warnings enabled
CXXFLAGS = -std=c++11 -Wall
NAME = testprog

all: $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(OBJ) -o $(NAME)

include ../makedef/makefile.rules