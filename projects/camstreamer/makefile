default:
	$(MAKE) -C code

.PHONY:clean 

clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C code
	$(MAKE) clean -C scripts
