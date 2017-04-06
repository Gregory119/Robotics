testprogs:
	$(MAKE) -C joystickserialreceiver
	$(MAKE) -C joystickserialtransmitter
	$(MAKE) -C joystick

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C joystickserialreceiver
	$(MAKE) clean -C joystickserialtransmitter
	$(MAKE) clean -C joystick
