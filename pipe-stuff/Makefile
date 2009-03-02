.PHONY: all clean cleanall

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../general-stuff
LDFLAGS = -L..


all: playpipe pipegen

../liborgan.a:
	$(MAKE) -C ../general-stuff liborgan

playpipe: ../liborgan.a
# REAL STUFF HERE

pipegen: ../liborgan.a
# REAL STUFF HERE


clean:
	rm -f *.o
	rm -f playpipe pipegen

cleanall: clean


