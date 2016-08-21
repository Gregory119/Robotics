all: 
	$(MAKE) -C coms
	$(MAKE) -C kern
	$(MAKE) -C joystick

.PHONY: clean
clean:
	$(MAKE) clean -C kern
	$(MAKE) clean -C coms
	$(MAKE) clean -C joystick
	$(MAKE) clean -C makedef
	$(RM) *~ *.o* *# *.core