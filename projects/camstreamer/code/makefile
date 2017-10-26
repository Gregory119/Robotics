BASE_DIR = ../../..

include $(BASE_DIR)/makedef/makefile.set

IS_TESTPROG=1
#creating test program name with cpp file in this directory
TEST_NAME = camstreamer

USES_LIBDGP=1
USES_LIBWIRINGPI=1

include $(BASE_DIR)/makedef/makefile.libs
include $(BASE_DIR)/makedef/makefile.rules

install: 
	cp $(TEST_NAME) /camstreamer/.
