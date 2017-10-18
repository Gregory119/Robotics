testprogs:
	$(MAKE) -C fastcontroller
	$(MAKE) -C modecontroller

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C fastcontroller
	$(MAKE) clean -C modecontroller
