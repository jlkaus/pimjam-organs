

.PHONY: all clean cleanall libmidi

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../organ-stuff -I../general-stuff -I../midi-stuff
CFLAGS = 
LDFLAGS = -L..
LIBS = -lorgan -lxml -lmidi

all: libmidi midi-test scom

../libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml

../liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan

FORCE:

libmidi: ../libmidi.a

# Actual targets
../libmidi.a: SComm.o Midi.o
	ar cr $@ $^
midi-test: $(LIBDEPS)
scom: scom.o
	$(CC) $(LDFLAGS) -o $@ $<

# source prereqs
midi-test.o: Scomm.H Midi.H
Scomm.o: Scomm.H
Midi.o: Midi.H

# implicit rules
%.o: %.C
	$(CXX) -c $(CXXFLAGS) -o $@ $<
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
%: %.o
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f *.o
	rm -f ../libmidi.a
	rm -f midi-test scom

cleanall: clean

