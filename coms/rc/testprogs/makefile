testprogs:
	$(MAKE) -C pwmreader
	$(MAKE) -C statepwmreader

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C pwmreader
	$(MAKE) clean -C statepwmreader
