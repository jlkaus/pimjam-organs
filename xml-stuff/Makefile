

.PHONY: all clean cleanall libxml

CC = gcc
CXX = g++
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP
LDFLAGS = -L..
LIBS =

all: libxml


libxml: ../libxml.a

# Actual targets
../libxml.a: ticpp.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
	ar cr $@ $^

# Source prereqs
ticpp.o: ticpp.h tinyxml.h ticpprc.h
tinystr.o: tinystr.h
tinyxml.o: tinyxml.h ticpprc.h
tinyxmlerror.o: tinyxml.h ticpprc.h
tinyxmlparser.o: tinyxml.h ticpprc.h

# implicit rules
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f ../libxml.a

cleanall: clean

