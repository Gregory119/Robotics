all: 
	$(MAKE) http
	$(MAKE) udp

.PHONY:clean testprogs

testprogs:
	$(MAKE) testprogs -C http

clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C http
	$(MAKE) clean -C udp
