.PHONY: clean all cleanall bootloader

CC = avr-gcc
CXX = avr-g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../corelib
LDFLAGS = -L..


all: bootloader

../corelib.a:
	$(MAKE) -C ../corelib

bootloader: ../corelib.a
# REAL STUFF HERE


clean:
	rm -f *.o

cleanall: clean
