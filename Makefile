
SYSTEMS=organ-stuff pipe-stuff midi-stuff firmware-stuff data general-stuff xml-stuff


.PHONY: all clean $(SYSTEMS) cleanall liborgan libmidi firmware organ libxml

all: firmware-stuff organ-stuff data pipe-stuff midi-stuff xml-stuff

firmware:
	$(MAKE) -C firmware-stuff firmware

liborgan:
	$(MAKE) -C general-stuff liborgan

libmidi:
	$(MAKE) -C midi-stuff libmidi

libxml:
	$(MAKE) -C xml-stuff libxml

organ:
	$(MAKE) -C organ-stuff organ-emu


$(SYSTEMS):
	$(MAKE) -C $@




clean:
	rm -f *.o

cleanall: clean
	@for dir in $(SYSTEMS); do $(MAKE) -C $$dir cleanall; done

