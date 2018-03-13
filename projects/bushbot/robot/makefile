BASE_DIR = ../../..

include $(BASE_DIR)/makedef/makefile.set

IS_TESTPROG=1
#creating test program name with cpp file in this directory
TEST_NAME = robot

USES_LIBCONTROL=1
USES_LIBJS=1
USES_LIBGP=1

include $(BASE_DIR)/makedef/makefile.libs
include $(BASE_DIR)/makedef/makefile.rules

install: 
	cp $(TEST_NAME) /Bushbot/.
