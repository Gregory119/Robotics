normal:
	$(MAKE) libs
	$(MAKE) testprogs
	$(MAKE) projects

.PHONY:clean libs testprogs projects all control

libs:
	$(MAKE) -C coms
	$(MAKE) -C kern
	$(MAKE) -C drivers
	$(MAKE) -C utils
	$(MAKE) -C core
	$(MAKE) -C control
	$(MAKE) -C peripherals

testprogs:
	$(MAKE) testprogs -C utils
	$(MAKE) testprogs -C kern
	$(MAKE) testprogs -C drivers
	$(MAKE) testprogs -C coms
	$(MAKE) testprogs -C core
	$(MAKE) testprogs -C control

projects:
	$(MAKE) -C projects

clean:
	$(MAKE) clean -C kern
	$(MAKE) clean -C coms
	$(MAKE) clean -C drivers
	$(MAKE) clean -C utils
	$(MAKE) clean -C core
	$(MAKE) clean -C control
	$(MAKE) clean -C makedef
	$(MAKE) clean -C projects
	$(MAKE) clean -C general_testprogs
	$(RM) *~ *.d* *.o* *# *.core
