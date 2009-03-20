.PHONY: all clean cleanall

CC = gcc
CXX = g++
CXXFLAGS = -D__STDC_LIMIT_MACROS -DTIXML_USE_TICPP -I../general-stuff -I../midi-stuff -I../xml-stuff
LDFLAGS = -L..


all: organ-emu organ-midi organ-create

../libmidi.a:
	$(MAKE) -C ../midi-stuff libmidi

../liborgan.a:
	$(MAKE) -C ../general-stuff liborgan

../libxml.a:
	$(MAKE) -C ../xml-stuff libxml

organ-emu: organ-emu.o ../liborgan.a ../libmidi.a ../libxml.a
	$(CXX) $(LDFLAGS) -c $@ $< -lorgan -lmidi -lxml
	
organ-emu.o: organ-emu.C ../liborgan.a ../libmidi.a ../libxml.a
	$(CXX) $(CXXFLAGS) -o $@ $<

organ-midi: organ-midi.o ../liborgan.a ../libmidi.a ../libxml.a
	$(CXX) $(LDFLAGS) -c $@ $< -lorgan -lmidi -lxml
organ-midi.o: organ-midi.C ../liborgan.a ../libmidi.a ../libxml.a
	$(CXX) $(CXXFLAGS) -o $@ $<

organ-create: organ-create.o ../liborgan.a ../libxml.a
	$(CXX) $(LDFLAGS) -o $@ $< -lorgan -lxml

organ-create.o: organ-create.C ../liborgan.a ../libxml.a
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o
	rm -f organ-emu organ-midi organ-create

cleanall: clean

