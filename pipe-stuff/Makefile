.PHONY: all clean cleanall

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS =  -D__STDC_LIMIT_MACROS -I../general-stuff -I../xml-stuff -I../midi-stuff
CFLAGS = 
LDFLAGS = -L..
LIBS = -lorgan -lxml -lmidi

all: playpipe pipegen playset dftwav wavdft rankgen playrank playbar

../liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan

../libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml

../libmidi.a: FORCE
	$(MAKE) -C ../midi-stuff libmidi

FORCE:

# actual targets
wavdft: $(LIBDEPS)
dftwav: $(LIBDEPS)
pipegen: $(LIBDEPS)
rankgen: $(LIBDEPS)
playpipe: $(LIBDEPS)
playset: $(LIBDEPS)
playrank: $(LIBDEPS)
playbar: $(LIBDEPS)

# source prerequisites

# implicit rules
%.o: %.C
	$(CXX) -c $(CXXFLAGS) -o $@ $<
%: %.o
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)


clean:
	rm -f *.o
	rm -f playpipe pipegen playset dftwav wavdft rankgen playrank playbar

cleanall: clean


