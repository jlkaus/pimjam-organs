
SYSTEMS=bootloader corelib

LIBS = -lcore

EXTRA_DEFS =

AVR_FREQ   = 20000000L
AVR_TARGET = atmega164p
STD_AVR_DEFS = -mmcu=$(AVR_TARGET) -DF_CPU=$(AVR_FREQ)

CC = /usr/local/avr/bin/avr-gcc
CXX = /usr/local/avr/bin/avr-g++
OBJCOPY = /usr/local/avr/bin/avr-objcopy
OBJDUMP = /usr/local/avr/bin/avr-objdump
CFLAGS = -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS) -Icorelib
CXXFLAGS = -D__STDC_LIMIT_MACROS -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS) -Icorelib
LDFLAGS = -L..

.PHONY: all clean $(SYSTEMS) cleanall firmware

all: bootloader firmware

libcore.a: FORCE
	$(MAKE) -C corelib

FORCE:

$(SYSTEMS): FORCE
	$(MAKE) -C $@

# Actual targets
firmware: libcore.a


# source prereqs


# implicit rules
%.elf: %.o
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
%.o: %.C
	$(CXX) -c $(CXXFLAGS) -o $@ $<
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@
%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@


clean:
	-rm -f *.o

cleanall: clean
	@for dir in $(SYSTEMS); do $(MAKE) -C $$dir cleanall; done
