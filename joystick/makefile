all: main.o joystick.o
	g++ main.o joystick.o -o testprog

main.o: main.cpp
	g++ -c main.cpp

joystick.o: joystick.h joystick.cpp
	g++ -c joystick.cpp

clean:
	rm *~ *.o *#