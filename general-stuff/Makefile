.PHONY: all clean cleanall liborgan

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../xml-stuff -I../midi-stuff -I../general-stuff
LDFLAGS = -L..
LIBS = -lorgan -lxml -lmidi

all: ../liborgan.a 

../libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml
../libmidi.a: FORCE
	$(MAKE) -C ../midi-stuff libmidi

FORCE:

liborgan: ../liborgan.a

# Actual targets
../liborgan.a: AlsaHelp.o Rank.o Pipe.o Organ.o Division.o PipeSet.o PlayControlBlock.o Coupler.o Effect.o Input.o Keyboard.o Stop.o Env.o
	ar cr $@ $^

# source prereqs
Env.o: Env.H
PlayControlBlock.o: PlayControlBlock.H Pipe.H Env.H
AlsaHelp.o: AlsaHelp.H Env.H
Rank.o: Rank.H PipeSet.H Env.H rankfile.H
Pipe.o: Pipe.H Env.H rankfile.H
Organ.o: Organ.H Division.H Input.H Env.H PlayControlBlock.H
Division.o: Division.H Rank.H Stop.H Coupler.H Effect.H Input.H Keyboard.H PlayControlBlock.H Env.H
PipeSet.o: PipeSet.H Pipe.H Env.H rankfile.H
Coupler.o: Coupler.H Env.H
Effect.o: Effect.H Env.H
Input.o: Input.H Env.H
Keyboard.o: Keyboard.H Env.H
Stop.o: Stop.H Env.H

# implicit rules
%.o: $.C
	$(CXX) -c $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f ../liborgan.a 

cleanall: clean


