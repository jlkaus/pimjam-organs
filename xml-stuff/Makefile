

.PHONY: all clean cleanall libxml

CC = gcc
CXX = g++
CXXFLAGS = -c -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP
LDFLAGS = -L..
LIBS =

all: libxml


libxml: ../libxml.a

../libxml.a: ticpp.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o
	ar cr $@ $^

ticpp.o: ticpp.cpp ticpp.h tinyxml.h ticpprc.h
	$(CXX) $(CXXFLAGS) -o $@ $<

tinystr.o: tinystr.cpp tinystr.h
	$(CXX) $(CXXFLAGS) -o $@ $<
tinyxml.o: tinyxml.cpp tinyxml.h ticpprc.h
	$(CXX) $(CXXFLAGS) -o $@ $<
tinyxmlerror.o: tinyxmlerror.cpp tinyxml.h ticpprc.h
	$(CXX) $(CXXFLAGS) -o $@ $<
tinyxmlparser.o: tinyxmlparser.cpp tinyxml.h ticpprc.h
	$(CXX) $(CXXFLAGS) -o $@ $<


clean:
	rm -f *.o
	rm -f ../libxml.a

cleanall: clean

