.PHONY: all clean cleanall

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../general-stuff ../xml-stuff
LDFLAGS = -L..


all: playpipe pipegen playset wavdft rankgen playrank playbar

../liborgan.a:
	$(MAKE) -C ../general-stuff liborgan

wavdft: ../liborgan.a
# REAL STUFF HERE

pipegen: ../liborgan.a
# REAL STUFF HERE

rankgen: ../liborgan.a
# REAL STUFF HERE

playpipe: ../liborgan.a
# REAL STUFF HERE

playset: ../liborgan.a
# REAL STUFF HERE

playrank: ../liborgan.a
# REAL STUFF HERE

playbar: ../liborgan.a
# REAL STUFF HERE

clean:
	rm -f *.o
	rm -f playpipe pipegen playset wavdft rankgen playrank playbar

cleanall: clean


