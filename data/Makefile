.PHONY: all clean cleanall rank-data pipe-data

all: rank-data

../bin/rankgen: FORCE
	$(MAKE) -C ../pipe-stuff rankgen
../bin/pipegen: FORCE
	$(MAKE) -C ../pipe-stuff pipegen
../bin/dftwav: FORCE
	$(MAKE) -C ../pipe-stuff dftwav

FORCE:

rank-data: pipe-data ../bin/rankgen


pipe-data: ../bin/pipegen ../bin/dftwav
	../bin/pipegen pure_440.dft 1 1
	../bin/dftwav pure_440.dft
	../bin/pipegen double_440.dft 2 1
	../bin/dftwav double_440.dft
	../bin/pipegen third_440.dft 3 1
	../bin/dftwav third_440.dft
	../bin/pipegen fourth_440.dft 4 1
	../bin/dftwav fourth_440.dft
	../bin/pipegen fifth_440.dft 5 1
	../bin/dftwav fifth_440.dft
	../bin/pipegen stopped_third_440.dft 3 0
	../bin/dftwav stopped_third_440.dft
	../bin/pipegen stopped_fifth_440.dft 5 0
	../bin/dftwav stopped_fifth_440.dft



clean:
	-rm -f *.pipe
	-rm -f *.rank

cleanall: clean

