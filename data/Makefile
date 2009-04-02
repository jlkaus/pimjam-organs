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

rank-data: pipe-data ../bin/rankgen


# parms:	max mutation harmonic (9)
#		detuning amount in cents (2)
#		note range, in octave numbers (0-9)
#		imperfection of stopping (0.25)
#            imperfection of overblowing (0.25)
#		decay rate step (0.50)
#		decay rate max (2.00)
#            cutoff frequency (22050)
#		limitation imperfection (0.25)
#		list of limitation harmonics to generate other than fake ones for L0p and L0i

dft-data: ../bin/dftgen
	../bin/dftgen 9 2 0 9 0.25 0.25 0.50 2.00 22050 0.25 3 5 13 37

pipe-data: dft-data ../bin/pipegen ../bin/dftwav
#	../bin/dftwav pure_440.dft
#	../bin/pipegen double_440.dft 2 1
#	../bin/dftwav double_440.dft
#	../bin/pipegen third_440.dft 3 1
#	../bin/dftwav third_440.dft
#	../bin/pipegen fourth_440.dft 4 1
#	../bin/dftwav fourth_440.dft
#	../bin/pipegen fifth_440.dft 5 1
#	../bin/dftwav fifth_440.dft
#	../bin/pipegen twentythird_440.dft 23 1
#	../bin/dftwav twentythird_440.dft
#	../bin/pipegen tenth_440.dft 10 1
#	../bin/dftwav tenth_440.dft
#	../bin/pipegen stopped_tenth_440.dft 10 0
#	../bin/dftwav stopped_tenth_440.dft
#	../bin/pipegen stopped_ninth_246.dft 9 0
#	../bin/dftwav stopped_ninth_246.dft
#	../bin/pipegen ninth_246.dft 9 1
#	../bin/dftwav ninth_246.dft
#	../bin/pipegen stopped_third_440.dft 3 0
#	../bin/dftwav stopped_third_440.dft
#	../bin/pipegen stopped_fifth_440.dft 5 0
#	../bin/dftwav stopped_fifth_440.dft
#	../bin/pipegen stopped_twentythird_440.dft 23 0
#	../bin/dftwav stopped_twentythird_440.dft




%.raw: %.dft
	../bin/dftwav $< 8000 2 0.1 1.5 44100

%.pipe: %.dft
	../bin/pipegen $< 1.0 5 0.05 1.1 44100

clean:
	-rm -f *.pipe
	-rm -f *.rank
	-rm -f *.dft
	-rm -rf THP_*


cleanall: clean

