normal:
	$(MAKE) libs
	$(MAKE) testprogs
	$(MAKE) projects

.PHONY:clean libs testprogs projects all control

libs:
	$(MAKE) -C core
	$(MAKE) -C utils
	$(MAKE) -C kern
	$(MAKE) -C drivers
	$(MAKE) -C coms
	$(MAKE) -C peripherals
	$(MAKE) -C control

testprogs:
	$(MAKE) testprogs -C utils
	$(MAKE) testprogs -C kern
	$(MAKE) testprogs -C drivers
	$(MAKE) testprogs -C coms
#	$(MAKE) testprogs -C core
	$(MAKE) testprogs -C control

projects:
	$(MAKE) -C projects

clean:
	$(MAKE) clean -C kern CLEANING=1
	$(MAKE) clean -C coms CLEANING=1
	$(MAKE) clean -C drivers CLEANING=1
	$(MAKE) clean -C utils CLEANING=1
	$(MAKE) clean -C core CLEANING=1
	$(MAKE) clean -C control CLEANING=1
	$(MAKE) clean -C makedef CLEANING=1
	$(MAKE) clean -C projects CLEANING=1
	$(MAKE) clean -C peripherals CLEANING=1
	$(MAKE) clean -C general_testprogs CLEANING=1
	$(RM) *~ *.d* *.o* *# *.core
