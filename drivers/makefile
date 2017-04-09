all: 
	$(MAKE) joystick
	$(MAKE) gopro

.PHONY:clean testprogs

testprogs:
	$(MAKE) testprogs -C joystick
	#$(MAKE) testprogs -C gopro

clean:
	$(MAKE) clean -C joystick
	$(MAKE) clean -C gopro
	$(RM) *~ *.d* *.o* *# *.core
