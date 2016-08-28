BASE_DIR = ../..

include $(BASE_DIR)/makedef/makefile.set

IS_TESTPROG=1
#creating test program names with ccp files in this directory
TEST_NAMES += $(SRC:.cpp=) 

USES_LIBCOMS=1

include $(BASE_DIR)/makedef/makefile.libs

joystickserialreceiver: $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(OBJ) -o $@

joystickserialtransmmitter: $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LIBS) $(OBJ) -o $@

include $(BASE_DIR)/makedef/makefile.rules
include $(BASE_DIR)/makedef/makefile.test