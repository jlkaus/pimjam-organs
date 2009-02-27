
SYSTEMS=bootloader corelib


.PHONY: all clean $(SYSTEMS) cleanall firmware

all: bootloader firmware

firmware: corelib 

bootloader: corelib


$(SYSTEMS):
	$(MAKE) -C $@

clean:
	rm -f *.o

cleanall: clean
	@for dir in $(SYSTEMS); do $(MAKE) -C $$dir cleanall; done
