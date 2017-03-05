testprogs:
	$(MAKE) -C joystickserialreceiver
	$(MAKE) -C joystickserialtransmitter

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C joystickserialreceiver
	$(MAKE) clean -C joystickserialtransmitter
