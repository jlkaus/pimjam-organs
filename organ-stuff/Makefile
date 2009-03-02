.PHONY: all clean cleanall

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../general-stuff -I../midi-stuff
LDFLAGS = -L..


all: organ-emu organ-test playrank playrange

../libmidi.a:
	$(MAKE) -C ../midi-stuff libmidi

../liborgan.a:
	$(MAKE) -C ../general-stuff liborgan

organ-emu: ../liborgan.a ../libmidi.a
# REAL STUFF HERE

organ-test: ../liborgan.a ../libmidi.a
# REAL STUFF HERE

playrange: ../liborgan.a
# REAL STUFF HERE

playrank: ../liborgan.a
# REAL STUFF HERE

clean:
	rm -f *.o
	rm -f organ-emu organ-test playrange playrank

cleanall: clean

