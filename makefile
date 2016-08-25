all: 
	$(MAKE) -C coms
	$(MAKE) -C kern
	$(MAKE) -C joystick
	$(MAKE) -C utils
	$(MAKE) testprogs

.PHONY=clean libs testprogs
libs:
	$(MAKE) -C coms
	$(MAKE) -C kern
	$(MAKE) -C joystick

testprogs:
	$(MAKE) testprogs -C coms
	$(MAKE) testprogs -C kern
	$(MAKE) testprogs -C joystick

clean:
	$(MAKE) clean -C kern
	$(MAKE) clean -C coms
	$(MAKE) clean -C joystick
	$(MAKE) clean -C utils
	$(MAKE) clean -C makedef
	$(RM) *~ *.d* *.o* *# *.core