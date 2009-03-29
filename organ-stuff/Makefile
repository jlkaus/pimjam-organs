.PHONY: all clean cleanall

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../general-stuff -I../midi-stuff -I../xml-stuff
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


# actual targets
organ-emu: $(LIBDEPS)
organ-midi: $(LIBDEPS)
organ-create: $(LIBDEPS)

# source prerequisites


# implicit rules
%.o: %.C
	$(CXX) -c $(CXXFLAGS) -o $@ $<
%::%.o
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)


clean:
	rm -f *.o
	rm -f organ-emu organ-midi organ-create

cleanall: clean

