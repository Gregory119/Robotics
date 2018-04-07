testprogs:
	$(MAKE) -C pwmreader
	$(MAKE) -C statepwmreader
	$(MAKE) -C ppmreader
	$(MAKE) -C stateppmreader

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C pwmreader
	$(MAKE) clean -C statepwmreader
	$(MAKE) clean -C ppmreader
	$(MAKE) clean -C stateppmreader
