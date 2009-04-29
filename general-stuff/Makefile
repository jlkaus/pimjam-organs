.PHONY: all clean cleanall liborgan

ACT_TARGETS =
LIB_SRC = AlsaHelp Rank Pipe Organ Division PlayControlBlock Coupler Effect Input Keyboard Stop Env Event
LIBS = -lorgan -lxml -lmidi

VPATH = ../lib
CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../xml-stuff -I../midi-stuff -I../general-stuff
LDFLAGS = -L../lib

all: ../lib/liborgan.a $(ACT_TARGETS)

../lib/libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml
../lib/libmidi.a: FORCE
	$(MAKE) -C ../midi-stuff libmidi
FORCE:
liborgan: ../lib/liborgan.a

# Actual targets
../lib/liborgan.a: $(LIB_SRC:=.o)
	ar cr $@ $^
$(ACT_TARGETS): $(LIBS)

# source prerequisites
-include $(ACT_TARGETS:%=../deps/%.d)
-include $(LIB_SRC:%=../deps/%.d)

# implicit rules
%.o: %.C
	$(CXX) -c $(CXXFLAGS) -o $@ $<
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
	-rm -f $(ACT_TARGETS) ../lib/liborgan.a 

cleanall: clean


