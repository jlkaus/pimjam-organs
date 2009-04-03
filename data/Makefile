.PHONY: all clean cleanall rank-data pipe-data

all: rank-data

../bin/rankgen: FORCE
	$(MAKE) -C ../pipe-stuff rankgen
../bin/dftgen: FORCE
	$(MAKE) -C ../pipe-stuff dftgen
../bin/pipegen: FORCE
	$(MAKE) -C ../pipe-stuff pipegen
../bin/dftwav: FORCE
	$(MAKE) -C ../pipe-stuff dftwav

FORCE:

rank-data:


pipe-data: 



%.dft: ../bin/dftgen
	../bin/dftgen $@ 2

%.raw: %.dft ../bin/dftwav
	../bin/dftwav $< 8000 0.25 0.08 1.4 44100

%.pipe: %.dft ../bin/pipegen
	../bin/pipegen $< 1.0 5 0.05 1.1 44100

clean:
	-rm -f *.pipe
	-rm -f *.rank
	-rm -f *.raw
	-rm -f *.dft
	-rm -rf XD*


cleanall: clean

