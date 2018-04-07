all: 
	$(MAKE) mapping
	$(MAKE) threading
	$(MAKE) filtering

.PHONY:clean testprogs

testprogs:
	$(MAKE) testprogs -C mapping
	$(MAKE) testprogs -C threading
	$(MAKE) testprogs -C filtering

clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C mapping
	$(MAKE) clean -C threading
	$(MAKE) clean -C filtering
