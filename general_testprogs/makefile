default:
	$(MAKE) -C simple
	$(MAKE) -C qt
	$(MAKE) -C threadpipe	

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C simple
	$(MAKE) clean -C qt
	$(MAKE) clean -C threadpipe
