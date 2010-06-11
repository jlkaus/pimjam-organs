.PHONY: all clean cleanall

ACT_TARGETS = organ-emu organ-midi organ-create
LIB_SRC =
LIBS = -lmidi -lorgan -lxml -lpthread

VPATH = ../lib
CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../general-stuff -I../midi-stuff -I../xml-stuff
LDFLAGS = -L../lib


all: $(ACT_TARGETS)

../lib/libmidi.a: FORCE
	$(MAKE) -C ../midi-stuff libmidi
../lib/liborgan.a: FORCE
	$(MAKE) -C ../general-stuff liborgan
../lib/libxml.a: FORCE
	$(MAKE) -C ../xml-stuff libxml
FORCE:

# actual targets
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
	-rm -f $(ACT_TARGETS)

cleanall: clean

