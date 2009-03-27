.PHONY: all clean cleanall rank-data pipe-data

all: rank-data

../pipe-stuff/rankgen: FORCE
	$(MAKE) -C ../pipe-stuff rankgen
../pipe-stuff/pipegen: FORCE
	$(MAKE) -C ../pipe-stuff pipegen

FORCE:

rank-data: pipe-data ../pipe-stuff/rankgen


pipe-data: ../pipe-stuff/pipegen
	../pipe-stuff/pipegen pure_440.dft 1 1
	../pipe-stuff/pipegen double_440.dft 2 1
	../pipe-stuff/pipegen third_440.dft 3 1
	../pipe-stuff/pipegen fourth_440.dft 4 1
	../pipe-stuff/pipegen fifth_440.dft 5 1
	../pipe-stuff/pipegen stopped_third_440.dft 3 0
	../pipe-stuff/pipegen stopped_fifth_440.dft 5 0



clean:
	rm -f *.pipe
	rm -f *.rank

cleanall: clean

