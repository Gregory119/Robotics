all: 
	$(MAKE) mapping
	$(MAKE) threading

.PHONY:clean testprogs

testprogs:
	$(MAKE) testprogs -C mapping
	$(MAKE) testprogs -C threading

clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C mapping
	$(MAKE) clean -C threading
