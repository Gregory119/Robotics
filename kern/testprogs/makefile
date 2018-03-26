testprogs:
	$(MAKE) -C stdkernel
	$(MAKE) -C asiokernel

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C stdkernel
	$(MAKE) clean -C asiokernel
