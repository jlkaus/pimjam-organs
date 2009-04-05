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

testing-raws: testing/XD100OD000ED000F100E005S005P0_L0020_261.626.raw \
	testing/XD100OD000ED000F100E005S005P0_L0020_293.665.raw\
	testing/XD100OD000ED000F100E005S005P0_L0020_329.628.raw\
	testing/XD100OD000ED000F100E005S005P0_L0020_349.228.raw\
	testing/XD100OD000ED000F100E005S005P0_L0020_391.995.raw\
	testing/XD100OD000ED000F100E005S005P0_L0020_440.000.raw\
	testing/XD100OD000ED000F100E005S005P0_L0020_493.884.raw\
	testing/XD100OD000ED000F100E005S005P0_L0020_523.251.raw


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

