all: 
	$(MAKE) mapping
	$(MAKE) threading
	$(MAKE) filtering
	$(MAKE) logging

.PHONY:clean testprogs

testprogs:
	$(MAKE) testprogs -C mapping

clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C mapping
	$(MAKE) clean -C threading
	$(MAKE) clean -C filtering
	$(MAKE) clean -C logging
