
SYSTEMS=bootloader corelib

LIBS = #-lcore

EXTRA_DEFS =

#AVR_FREQ   = 16000000L
AVR_FREQ   = 20000000L
AVR_TARGET = atmega164p
STD_AVR_DEFS = -mmcu=$(AVR_TARGET) -DF_CPU=$(AVR_FREQ)

CC = /usr/local/avr/bin/avr-gcc
CXX = /usr/local/avr/bin/avr-g++
OBJCOPY = /usr/local/avr/bin/avr-objcopy
OBJDUMP = /usr/local/avr/bin/avr-objdump
CFLAGS = -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS) -Icorelib -std=gnu99 
CXXFLAGS = -D__STDC_LIMIT_MACROS -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS) -Icorelib
LDFLAGS = $(STD_AVR_DEFS) -L.

.PHONY: all clean $(SYSTEMS) cleanall firmware

all: bootloader firmware eeprom

libcore.a: FORCE
	$(MAKE) -C corelib

FORCE:

$(SYSTEMS): FORCE
	$(MAKE) -C $@

# Actual targets
firmware: organbop.hex

eeprom: organbop_eeprom.hex

# source prereqs
organbop.elf: organbop.o

organbop.o: config_structures.h config_defaults.h

# implicit rules
%.elf: %.o
	$(CC) $(LDFLAGS) -o $@ $< $(LIBS)
%.o: %.c
	$(CC) -c $(CFLAGS) -g -o $@ $<
%.o: %.C
	$(CXX) -c $(CXXFLAGS) -g -o $@ $<
%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@
%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
%_eeprom.hex: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@
%_eeprom.srec: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O srec $< $@
%_eeprom.bin: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

clean:
	-rm -f *.o *.elf *.lst *.hex *.srec *.bin *.a

cleanall: clean
	@for dir in $(SYSTEMS); do $(MAKE) -C $$dir cleanall; done
