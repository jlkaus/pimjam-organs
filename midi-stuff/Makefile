

.PHONY: all clean cleanall libmidi

ACT_TARGETS = midi-test
LIB_SRC = SComm Midi
LIBS = -lorgan -lxml -lmidi

VPATH = ../lib
CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -I../organ-stuff -I../general-stuff -I../midi-stuff
CFLAGS = 
LDFLAGS = -L../lib

all: libmidi $(ACT_TARGETS) scom

../lib/libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml
../lib/liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan
FORCE:
libmidi: ../lib/libmidi.a

# Actual targets
../lib/libmidi.a: $(LIB_SRC:=.o)
	ar cr $@ $^
$(ACT_TARGETS): $(LIBS)
scom: scom.o
	$(CC) $(LDFLAGS) -o $@ $<
	cp $@ ../bin/$@
midi-test: midi-test.o
	$(CC) $(LDFLAGS) -o $@ $<
	cp $@ ../bin/$@

# source prereqs
-include $(ACT_TARGETS:%=../deps/%.d)
-include $(LIB_SRC:%=../deps/%.d)

# implicit rules
%.o: %.C
	$(CXX) -c $(CXXFLAGS) -o $@ $<
%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
../deps/%.d: %.C
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
%: %.o
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)
	cp $@ ../bin/$@

clean:
	-rm -f *.o
	-rm -f ../lib/libmidi.a
	-rm -f $(ACT_TARGETS) scom

cleanall: clean

