bushbot:
	$(MAKE) -C robot
	$(MAKE) -C basestation

.PHONY=clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C robot
	$(MAKE) clean -C basestation
