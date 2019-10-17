
.PHONY:  doxy doxy.clean

doxy:
	$(AT)$(MAKE) -s -C doxy/ doxy.all

doxy.clean:
	$(AT)$(MAKE) -s -C doxy/ clean

