.PHONY: clean all cleanall bootloader

LIBS =

LED_FLASHES = 3
MAX_TIME_COUNT = '8000000L>>1'

EXTRA_DEFS = -DNUM_LED_FLASHES=$(LED_FLASHES) -DMAX_TIME_COUNT=$(MAX_TIME_COUNT)

AVR_FREQ   = 20000000L
AVR_TARGET = atmega164p
STD_AVR_DEFS = -mmcu=$(AVR_TARGET) -DF_CPU=$(AVR_FREQ)

CC = /usr/local/avr/bin/avr-gcc
CXX = /usr/local/avr/bin/avr-g++
OBJCOPY = /usr/local/avr/bin/avr-objcopy
OBJDUMP = /usr/local/avr/bin/avr-objdump
CFLAGS = -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS) -I../corelib
CXXFLAGS = -D__STDC_LIMIT_MACROS -Wall -Os $(STD_AVR_DEFS) $(EXTRA_DEFS) -I../corelib
LDFLAGS = -L.. -Wl,--section-start=.text=0x0000


all: bootloader

# Actual targets
bootloader: ATmegaBOOT.hex

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
	-rm -f *.o *.hex *.lst *.srec *.elf *.bin

cleanall: clean
