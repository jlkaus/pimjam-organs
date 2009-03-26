.PHONY: clean all cleanall bootloader

CC = /usr/local/avr/bin/avr-gcc
CXX = /usr/local/avr/bin/avr-g++
OBJCOPY = /usr/local/avr/bin/avr-objcopy
OBJDUMP = /usr/local/avr/bin/avr-objdump

AVR_FREQ   = 20000000L
LED_FLASHES = 3
MAX_TIME_COUNT = '8000000L>>1'

CFLAGS = -c -Wall -Os -mmcu=atmega164p -DF_CPU=$(AVR_FREQ) -DMAX_TIME_COUNT=$(MAX_TIME_COUNT) -DNUM_LED_FLASHES=$(LED_FLASHES)
CXXFLAGS = -D__STDC_LIMIT_MACROS 
LDFLAGS = -L.. -Wl,--section-start=.text=0x0000

all: bootloader

bootloader: ATmegaBOOT.hex

%.elf: %.o
	$(CC) $(LDFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

clean:
	rm -f *.o *.hex *.lst *.srec *.elf *.bin

cleanall: clean
