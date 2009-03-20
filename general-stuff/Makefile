.PHONY: all clean cleanall liborgan

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../xml-stuff
LDFLAGS = -L..


all: ../liborgan.a 

liborgan: ../liborgan.a

../liborgan.a: AlsaHelp.o Rank.o Pipe.o Organ.o Division.o PipeSet.o AlsaPlayControlBlock.o Coupler.o Effect.o Input.o Keyboard.o Stop.o Env.o
	ar cr $@ $^

Env.o: Env.C Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
AlsaPlayControlBlock.o: AlsaPlayControlBlock.C AlsaPlayControlBlock.H Pipe.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
AlsaHelp.o: AlsaHelp.C AlsaHelp.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Rank.o: Rank.C Rank.H PipeSet.H Env.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Pipe.o: Pipe.C Pipe.H Env.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Organ.o: Organ.C Organ.H Division.H Input.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Division.o: Division.C Division.H Rank.H Stop.H Coupler.H Effect.H Input.H Keyboard.H AlsaPlayControlBlock.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
PipeSet.o: PipeSet.C PipeSet.H Pipe.H Env.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Coupler.o: Coupler.C Coupler.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Effect.o: Effect.C Effect.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Input.o: Input.C Input.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Keyboard.o: Keyboard.C Keyboard.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Stop.o: Stop.C Stop.H Env.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<


clean:
	rm -f *.o
	rm -f ../liborgan.a 

cleanall: clean


