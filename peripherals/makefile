default:
	$(MAKE) -C wiringpi
	$(MAKE) -C wifi

.PHONY:clean
clean:
	$(RM) *~ *.d* *.o* *# *.core
	$(MAKE) clean -C wiringpi
	$(MAKE) clean -C wifi
