.PHONY: all clean cleanall

CC = gcc
CXX = g++
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -I../general-stuff -I../xml-stuff
LDFLAGS = -L..


all: playpipe pipegen playset wavdft rankgen playrank playbar

../liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan

FORCE:

wavdft: ../liborgan.a
# REAL STUFF HERE

pipegen: pipegen.o ../liborgan.a
	$(CXX) $(LDFLAGS) -o $@ $<

pipegen.o: pipegen.C
	$(CXX) $(CXXFLAGS) -o $@ $<

rankgen: ../liborgan.a
# REAL STUFF HERE

playpipe: ../liborgan.a
# REAL STUFF HERE

playset: playset.o ../liborgan.a
	$(CXX) $(LDFLAGS) -lorgan -o $@ $<

playset.o: playset.C
	$(CXX) $(CXXFLAGS) -o $@ $<

playrank: ../liborgan.a
# REAL STUFF HERE

playbar: ../liborgan.a
# REAL STUFF HERE

clean:
	rm -f *.o
	rm -f playpipe pipegen playset wavdft rankgen playrank playbar

cleanall: clean


