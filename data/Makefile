.PHONY: all clean cleanall rank-data pipe-data

all: rank-data

../bin/rankgen: FORCE
	$(MAKE) -C ../pipe-stuff rankgen
../bin/pipegen: FORCE
	$(MAKE) -C ../pipe-stuff pipegen

FORCE:

rank-data: pipe-data ../bin/rankgen


pipe-data: ../bin/pipegen
	../bin/pipegen pure_440.dft 1 1
	../bin/pipegen double_440.dft 2 1
	../bin/pipegen third_440.dft 3 1
	../bin/pipegen fourth_440.dft 4 1
	../bin/pipegen fifth_440.dft 5 1
	../bin/pipegen stopped_third_440.dft 3 0
	../bin/pipegen stopped_fifth_440.dft 5 0



clean:
	-rm -f *.pipe
	-rm -f *.rank

cleanall: clean

