.PHONY: all clean cleanall

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -I../general-stuff -I../xml-stuff -I../midi-stuff
CFLAGS = -c
LDFLAGS = -L..
LIBS = -lorgan -lxml -lmidi

all: playpipe pipegen playset wavdft rankgen playrank playbar

../liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan

../libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml

../libmidi.a: FORCE
	$(MAKE) -C ../midi-stuff libmidi

FORCE:

wavdft: $(LIBDEPS)
# REAL STUFF HERE

pipegen: pipegen.o $(LIBDEPS)
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)

pipegen.o: pipegen.C
	$(CXX) $(CXXFLAGS) -o $@ $<

rankgen: $(LIBDEPS)
# REAL STUFF HERE

playpipe: $(LIBDEPS)
# REAL STUFF HERE

playset: playset.o $(LIBDEPS)
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)

playset.o: playset.C
	$(CXX) $(CXXFLAGS) -o $@ $<

playrank: $(LIBDEPS)
# REAL STUFF HERE

playbar: $(LIBDEPS)
# REAL STUFF HERE

clean:
	rm -f *.o
	rm -f playpipe pipegen playset wavdft rankgen playrank playbar

cleanall: clean


