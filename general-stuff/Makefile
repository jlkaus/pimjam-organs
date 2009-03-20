.PHONY: all clean cleanall liborgan

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../xml-stuff
LDFLAGS = -L..


all: ../liborgan.a readorgan

liborgan: ../liborgan.a

../liborgan.a: AlsaHelp.o Rank.o Pipe.o Organ.o Division.o PipeSet.o AlsaPlayControlBlock.o Coupler.o Effect.o Input.o Keyboard.o Stop.o
	ar cr $@ $^

readorgan: readorgan.o ../liborgan.a ../libxml.a
	$(CXX) $(LDFLAGS) -o $@ $< -lorgan -lxml

readorgan.o: readorgan.C Organ.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<

AlsaPlayControlBlock.o: AlsaPlayControlBlock.C AlsaPlayControlBlock.H Pipe.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
AlsaHelp.o: AlsaHelp.C AlsaHelp.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Rank.o: Rank.C Rank.H PipeSet.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Pipe.o: Pipe.C Pipe.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Organ.o: Organ.C Organ.H Division.H Input.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Division.o: Division.C Division.H Rank.H Stop.H Coupler.H Effect.H Input.H Keyboard.H AlsaPlayControlBlock.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
PipeSet.o: PipeSet.C PipeSet.H Pipe.H rankfile.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Coupler.o: Coupler.C Coupler.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Effect.o: Effect.C Effect.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Input.o: Input.C Input.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Keyboard.o: Keyboard.C Keyboard.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<
Stop.o: Stop.C Stop.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<


clean:
	rm -f *.o
	rm -f ../liborgan.a readorgan

cleanall: clean


