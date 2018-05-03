TOP_DIR:=$(shell pwd)

normal:
	$(MAKE) libs TOP_DIR=$(TOP_DIR)
	$(MAKE) testprogs TOP_DIR=$(TOP_DIR)
	$(MAKE) projects TOP_DIR=$(TOP_DIR)

.PHONY:clean libs testprogs projects all control

libs:
	$(MAKE) -C core TOP_DIR=$(TOP_DIR)
	$(MAKE) -C utils TOP_DIR=$(TOP_DIR)
	$(MAKE) -C kern TOP_DIR=$(TOP_DIR)
	$(MAKE) -C drivers TOP_DIR=$(TOP_DIR)
	$(MAKE) -C coms TOP_DIR=$(TOP_DIR)
	$(MAKE) -C peripherals TOP_DIR=$(TOP_DIR)
	$(MAKE) -C control TOP_DIR=$(TOP_DIR)

testprogs:
	$(MAKE) testprogs -C utils TOP_DIR=$(TOP_DIR)
	$(MAKE) testprogs -C kern TOP_DIR=$(TOP_DIR)
	$(MAKE) testprogs -C drivers TOP_DIR=$(TOP_DIR)
	$(MAKE) testprogs -C coms TOP_DIR=$(TOP_DIR)
#	$(MAKE) testprogs -C core
	$(MAKE) testprogs -C control TOP_DIR=$(TOP_DIR)

projects:
	$(MAKE) -C projects TOP_DIR=$(TOP_DIR)

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
	$(MAKE) clean -C howtoguides CLEANING=1
	$(RM) *~ *.d* *.o* *# *.core
