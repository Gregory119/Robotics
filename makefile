all: 
	$(MAKE) libs

.PHONY=clean libs testprogs
libs:
	$(MAKE) -C coms
	$(MAKE) -C kern
	$(MAKE) -C joystick
	$(MAKE) -C utils

testprogs:
	$(MAKE) testprogs -C utils
	$(MAKE) testprogs -C kern
	$(MAKE) testprogs -C joystick
	$(MAKE) testprogs -C coms

clean:
	$(MAKE) clean -C kern
	$(MAKE) clean -C coms
	$(MAKE) clean -C joystick
	$(MAKE) clean -C utils
	$(MAKE) clean -C makedef
	$(RM) *~ *.d* *.o* *# *.core