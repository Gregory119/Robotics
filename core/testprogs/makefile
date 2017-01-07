testprogs:
	$(MAKE) -C softservo
	$(MAKE) -C hardservo

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C softservo
	$(MAKE) clean -C hardservo
