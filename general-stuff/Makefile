.PHONY: all clean cleanall liborgan

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../xml-stuff
LDFLAGS = -L..


all: ../liborgan.a

liborgan: ../liborgan.a

../liborgan.a: AlsaHelp.o Rank.o Pipe.o Organ.o Division.o PipeSet.o AlsaPlayControlBlock.o
	ar cr $@ $^

AlsaPlayControlBlock.o: AlsaPlayControlBlock.C AlsaPlayControlBlock.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
AlsaHelp.o: AlsaHelp.C AlsaHelp.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Rank.o: Rank.C Rank.H Pipe.H rankfile.H PipeSet.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Pipe.o: Pipe.C Pipe.H Rank.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Organ.o: Organ.C Organ.H Division.H 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Division.o: Division.C Division.H Rank.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
PipeSet.o: PipeSet.C PipeSet.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<



clean:
	rm -f *.o
	rm -f ../liborgan.a

cleanall: clean


