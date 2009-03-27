.PHONY: all clean cleanall

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../general-stuff -I../midi-stuff -I../xml-stuff
LDFLAGS = -L..
LIBS = -lmidi -lorgan -lxml

all: organ-emu organ-midi organ-create

../libmidi.a: FORCE
	$(MAKE) -C ../midi-stuff libmidi

../liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan

../libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml

FORCE:

organ-emu: organ-emu.o $(LIBDEPS)
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)

organ-emu.o: organ-emu.C
	$(CXX) $(CXXFLAGS) -o $@ $<

organ-midi: organ-midi.o $(LIBDEPS)
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)
organ-midi.o: organ-midi.C
	$(CXX) $(CXXFLAGS) -o $@ $<

organ-create: organ-create.o $(LIBDEPS)
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)

organ-create.o: organ-create.C
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f organ-emu organ-midi organ-create

cleanall: clean

