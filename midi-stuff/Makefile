

.PHONY: all clean cleanall libmidi

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS
LDFLAGS = -L..

all: libmidi midi-test scom


libmidi: ../libmidi.a

../libmidi.a: SComm.o Midi.o
	ar cr $@ $^

Scomm.o: Scomm.C Scomm.H
	$(CXX) $(CXXFLAGS) -o $@ $<
Midi.o: Midi.C Midi.H
	$(CXX) $(CXXFLAGS) -o $@ $<

midi-test: midi-test.o ../libmidi.a
	$(CXX) $(LDFLAGS) -o $@ $<

midi-test.o: midi-test.C Scomm.H Midi.H
	$(CXX) $(CXXFLAGS) -c -o $@ $<

scom: scom.o
	$(CC) $(LDFLAGS) -o $@ $<

scom.o: scom.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f ../libmidi.a
	rm -f midi-test scom

cleanall: clean

