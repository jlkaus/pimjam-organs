.PHONY: all clean cleanall rank-data pipe-data

all: rank-data

../pipe-stuff/rankgen: FORCE
	$(MAKE) -C ../pipe-stuff rankgen
../pipe-stuff/pipegen: FORCE
	$(MAKE) -C ../pipe-stuff pipegen

FORCE:

rank-data: pipe-data ../pipe-stuff/rankgen


pipe-data: ../pipe-stuff/pipegen
	../pipe-stuff/pipegen dulciana_440.dft



clean:
	rm -f *.pipe
	rm -f *.rank

cleanall: clean

