testprogs:
	$(MAKE) -C softservo
	$(MAKE) -C hardservo
	$(MAKE) -C stepvelocity

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C softservo
	$(MAKE) clean -C hardservo
	$(MAKE) clean -C stepvelocity
