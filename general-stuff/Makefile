.PHONY: all clean cleanall liborgan

LIBDEPS = ../liborgan.a ../libxml.a ../libmidi.a

CC = gcc
CXX = g++
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../xml-stuff -I../midi-stuff -I../general-stuff
LDFLAGS = -L..
LIBS = -lorgan -lxml -lmidi

all: ../liborgan.a 

../libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml
../libmidi.a: FORCE
	$(MAKE) -C ../midi-stuff libmidi

FORCE:

liborgan: ../liborgan.a

../liborgan.a: AlsaHelp.o Rank.o Pipe.o Organ.o Division.o PipeSet.o PlayControlBlock.o Coupler.o Effect.o Input.o Keyboard.o Stop.o Env.o
	ar cr $@ $^

Env.o: Env.C Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
PlayControlBlock.o: PlayControlBlock.C PlayControlBlock.H Pipe.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
AlsaHelp.o: AlsaHelp.C AlsaHelp.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Rank.o: Rank.C Rank.H PipeSet.H Env.H rankfile.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Pipe.o: Pipe.C Pipe.H Env.H rankfile.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Organ.o: Organ.C Organ.H Division.H Input.H Env.H PlayControlBlock.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Division.o: Division.C Division.H Rank.H Stop.H Coupler.H Effect.H Input.H Keyboard.H PlayControlBlock.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
PipeSet.o: PipeSet.C PipeSet.H Pipe.H Env.H rankfile.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Coupler.o: Coupler.C Coupler.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Effect.o: Effect.C Effect.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Input.o: Input.C Input.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Keyboard.o: Keyboard.C Keyboard.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Stop.o: Stop.C Stop.H Env.H
	$(CXX) $(CXXFLAGS) -o $@ $<


clean:
	rm -f *.o
	rm -f ../liborgan.a 

cleanall: clean


