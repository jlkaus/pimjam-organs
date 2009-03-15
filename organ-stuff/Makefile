.PHONY: all clean cleanall

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../general-stuff -I../midi-stuff -I../xml-stuff
LDFLAGS = -L..


all: organ-emu organ-midi

../libmidi.a:
	$(MAKE) -C ../midi-stuff libmidi

../liborgan.a:
	$(MAKE) -C ../general-stuff liborgan

organ-emu: ../liborgan.a ../libmidi.a
# REAL STUFF HERE

organ-midi: ../liborgan.a ../libmidi.a
# REAL STUFF HERE

clean:
	rm -f *.o
	rm -f organ-emu organ-midi

cleanall: clean

