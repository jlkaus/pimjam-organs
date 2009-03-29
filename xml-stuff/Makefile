

.PHONY: all clean cleanall libxml

ACT_TARGETS =
LIB_SRC = ticpp tinystr tinyxml tinyxmlerror tinyxmlparser
LIBS =

VPATH = ../lib
CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP
LDFLAGS = -L../lib

all: libxml

libxml: ../lib/libxml.a

# Actual targets
../lib/libxml.a: $(LIB_SRC:=.o)
	ar cr $@ $^
$(ACT_TARGETS): $(LIBS)

# Source prereqs
-include $(ACT_TARGETS:%=../deps/%.d)
-include $(LIB_SRC:%=../deps/%.d)

# implicit rules
%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<
../deps/%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
%: %.o
	$(CXX) $(LDFLAGS) -o $@ $< $(LIBS)
	cp $@ ../bin/$@

clean:
	-rm -f *.o
	-rm -f ../lib/libxml.a $(ACT_TARGETS)

cleanall: clean

