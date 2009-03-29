.PHONY: clean all cleanall corelib

LIBS =

EXTRA_DEFS =

AVR_FREQ   = 20000000L
AVR_TARGET = atmega164p
STD_AVR_DEFS = -mmcu=$(AVR_TARGET) -DF_CPU=$(AVR_FREQ)

CC = /usr/local/avr/bin/avr-gcc
CXX = /usr/local/avr/bin/avr-g++
OBJCOPY = /usr/local/avr/bin/avr-objcopy
OBJDUMP = /usr/local/avr/bin/avr-objdump
CFLAGS = -c -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS)
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS)
LDFLAGS = -L..


all: corelib

corelib: ../libcore.a

# Actual targets
../libcore.a: 
	ar cr $@ $^

# source prereqs


# implicit rules
%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<
%.o: %.C
	$(CCC) $(CXXFLAGS) -o $@ $<


clean:
	rm -f *.o
	rm -f ../libcore.a

cleanall: clean
