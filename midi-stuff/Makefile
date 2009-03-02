

.PHONY: all clean cleanall libmidi

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS
LDFLAGS = -L..

all: libmidi midi-test

libmidi: ../libmidi.a

../libmidi.a:
# REAL STUFF HERE

midi-test: libmidi
# REAL STUFF HERE

clean:
	rm -f *.o
	rm -f ../libmidi.a
	rm -f midi-test

cleanall: clean

