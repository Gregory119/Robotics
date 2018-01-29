all: 
	$(MAKE) joystick
	$(MAKE) gopro
	$(MAKE) led

.PHONY:clean testprogs

testprogs:
	$(MAKE) testprogs -C joystick
	$(MAKE) testprogs -C gopro
	$(MAKE) testprogs -C led

clean:
	$(MAKE) clean -C joystick
	$(MAKE) clean -C gopro
	$(MAKE) clean -C led
	$(RM) *~ *.d* *.o* *# *.core
