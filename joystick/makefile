OBJ = main.o joystick.o common.o
CXX = g++
# use this to include header files in different directories
INCLUDES = 
LIBS = -lpthread
# use -std=c++11 => c++11, -Wall => all warnings enabled
CXXFLAGS = -std=c++11 -Wall
NAME = testprog

all: $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(OBJ) -o $(NAME)

#overwriting the default implicit rule to create a .o file from a .cpp file
%.o : %.cpp 
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<
# $< is an implicit variable for the first prerequisite. Because it is an implicit rule, 
# in this case there is only one prerequisite (source file) for each required object file

clean:
	$(RM) *~ *.o *# core *.core $(NAME)