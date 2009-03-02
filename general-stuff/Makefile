.PHONY: all clean cleanall liborgan

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS
LDFLAGS =


all: ../liborgan.a

liborgan: ../liborgan.a

../liborgan.a: AlsaHelp.o Rank.o Pipe.o
# REAL STUFF HERE

AlsaHelp.o: AlsaHelp.C AlsaHelp.H
Rank.o: Rank.C Rank.H Pipe.H rankfile.H
Pipe.o: Pipe.C Pipe.H Rank.H rankfile.H


clean:
	rm -f *.o
	rm -f ../liborgan.a

cleanall: clean


