all: 
	$(MAKE) http
	$(MAKE) udp
	$(MAKE) rc

.PHONY:clean testprogs

testprogs:
	$(MAKE) testprogs -C http

clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C http
	$(MAKE) clean -C udp
	$(MAKE) clean -C rc
