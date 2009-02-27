
SYSTEMS=organ-stuff pipe-stuff midi-stuff firmware-stuff data


.PHONY: all clean $(SYSTEMS) cleanall

all: firmware-stuff organ-stuff data

organ-stuff: midi-stuff pipe-stuff

data: pipe-stuff

$(SYSTEMS):
	$(MAKE) -C $@

clean:
	rm -f *.o

cleanall: clean
	@for dir in $(SYSTEMS); do $(MAKE) -C $$dir cleanall; done

