BASE_DIR = ../../..

include $(BASE_DIR)/makedef/makefile.set

IS_TESTPROG=1
#creating test program name with cpp file in this directory
TEST_NAME = actioncamstreamer

USES_LIBGP=1
USES_LIBWIRINGPI=1
USES_LIBLED=1
USES_LIBWIFI=1
USES_LIBCRC=1
USES_LIBUTILLOGGING=1

ifdef PAID
USES_LIBCRYPT=1
CXXFLAGS+=-DLICENSED
else
OBJ_EXCLUDES:=license
CXXFLAGS+=-DEXCLUDE_VIDEO_MODE # The GoPro mode controller will not include video in the next mode command.
endif

include $(BASE_DIR)/makedef/makefile.libs
include $(BASE_DIR)/makedef/makefile.rules

install: 
	cp $(TEST_NAME) /actioncamstreamer/.
