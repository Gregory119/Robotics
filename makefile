all: 
	$(MAKE) libs
	$(MAKE) testprogs
	$(MAKE) projects

.PHONY=clean libs testprogs projects
libs:
	$(MAKE) -C coms
	$(MAKE) -C kern
	$(MAKE) -C joystick
	$(MAKE) -C utils
	$(MAKE) -C core

testprogs:
	$(MAKE) testprogs -C utils
	$(MAKE) testprogs -C kern
	$(MAKE) testprogs -C joystick
	$(MAKE) testprogs -C coms
	$(MAKE) testprogs -C core

projects:
	$(MAKE) -C projects

clean:
	$(MAKE) clean -C kern
	$(MAKE) clean -C coms
	$(MAKE) clean -C joystick
	$(MAKE) clean -C utils
	$(MAKE) clean -C core
	$(MAKE) clean -C makedef
	$(MAKE) clean -C projects
	$(RM) *~ *.d* *.o* *# *.core
