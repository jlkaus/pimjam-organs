
SYSTEMS=bootloader corelib

CC = avr-gcc
CXX = avr-g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -Icorelib
LDFLAGS = 


.PHONY: all clean $(SYSTEMS) cleanall firmware

all: bootloader firmware

corelib.a:
	$(MAKE) -C corelib

firmware: corelib.a
# REAL STUFF HERE

$(SYSTEMS):
	$(MAKE) -C $@

clean:
	rm -f *.o

cleanall: clean
	@for dir in $(SYSTEMS); do $(MAKE) -C $$dir cleanall; done
