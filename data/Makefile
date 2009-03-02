.PHONY: all clean cleanall rank-data pipe-data

all: rank-data

rank-data: pipe-data
# REAL STUFF HERE

pipe-data:
	$(MAKE) -C ../pipe-stuff
# REAL STUFF HERE


clean:
	rm -f *.pipe
	rm -f *.rank

cleanall: clean

