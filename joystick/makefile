CXXFLAGS = -std=c++11

all: main.o joystick.o common.o
	g++ $(CXXFLAGS) main.o joystick.o common.o -o testprog

main.o: main.cpp
	g++ $(CXXFLAGS) -c main.cpp

joystick.o: joystick.h joystick.cpp
	g++ $(CXXFLAGS) -c joystick.cpp

common.o: common.h common.cpp
	g++ $(CXXFLAGS) -c common.cpp

clean:
	rm *~ *.o *#