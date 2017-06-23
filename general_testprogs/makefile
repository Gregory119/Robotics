default:
	$(MAKE) -C simple
	$(MAKE) -C qt

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C simple
	$(MAKE) clean -C qt
