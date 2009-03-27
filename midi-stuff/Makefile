

.PHONY: all clean cleanall libmidi

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -I../organ-stuff -I../general-stuff -I../midi-stuff
CFLAGS = -c
LDFLAGS = -L..
LIBS = -lorgan -lxml -lmidi

all: libmidi midi-test scom

../libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml

../liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan

FORCE:

libmidi: ../libmidi.a

../libmidi.a: SComm.o Midi.o
	ar cr $@ $^

Scomm.o: Scomm.C Scomm.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Midi.o: Midi.C Midi.H
	$(CXX) $(CXXFLAGS) -o $@ $<

midi-test: midi-test.o $(LIBDEPS)
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)

midi-test.o: midi-test.C Scomm.H Midi.H
	$(CXX) $(CXXFLAGS) -o $@ $<

scom: scom.o
	$(CC) $(LDFLAGS) -o $@ $<

scom.o: scom.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f ../libmidi.a
	rm -f midi-test scom

cleanall: clean

