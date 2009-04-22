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
../bin/wavdft: FORCE
	$(MAKE) -C ../pipe-stuff wavdft
FORCE:

rank-data:


pipe-data: 


Principal_8.rsf: OpenDiapason.fis
	../scripts/rsfgen.pl -o $@ OpenDiapason.fis:8 -k61 -a0 -b0 -d0

# Flues
#   Unstopped
#     Medium
# typisch 8,4,2,1,16
Diapason_%.rsf:
	../scripts/rsfgen.pl -o $@ Diapason.fis -k61 -a0 -b0 -d0

#     Slightly narrower
# typisch 8
Geigenprincipal_%.rsf:

#   Stopped
#     Slightly broader
# typisch 8,16,32
Bourdon_%.rsf:

#   Incompletely stopped
# typisch 4
Rohrfloete_%.rsf:

#   Unstopped
#     Wider
# typisch 8,16,4,2,1
Flute_%.rsf:

#     Narrower
# typisch 8,16
Gamba_%.rsf:

#     Narrower, but a bit wider than gamba
# typisch 8,4
Salicional_%.rsf:

#   Stopped
#     gamba sized
# typisch 8
Quintation_%.rsf:

#   Unstopped
#     very narrow
# typisch 8
Dulciana_%.rsf:

#     narrow and narrowing to top
Gemshorn_%.rsf:

#   Unstopped
#     gamba sized
# Slightly detuned (sharp)
VoxCeleste_%.rsf:

# Reeds
#   Unstopped
#     normal length, widening to top
# typisch 8,16,4
Trumpet_%.rsf:

#     normal length and straight, but widening cone at the top
# typisch 8
Oboe_%.rsf:

#     just like Oboe, but slightly wider resonator
# typisch 16
Bassoon_%.rsf:

#     half length resonator
# typisch 8
Clarinet_%.rsf:

#     very short resonator
VoxHumana_%.rsf:



%.fis: %.ssf
	../scripts/ssffis.pl $< -o $@ 
%.fis: ../bin/dftgen
	../bin/dftgen -o $@ -v2
%.rank: %.rsf ../bin/rankgen
	-include $< 
	../bin/rankgen $< -o $@
%.raw: FORCE ../bin/dftwav
	../bin/dftwav -o $@ -v2 -r44100 -n8000 -a0.05 -d0.05 -l1.0
%.pipe: FORCE ../bin/pipegen
	../bin/pipegen -o $@ -v2 -r44100 -n1.0 -l1.0
%.wav: FORCE ../bin/dftwav
	../bin/dftwav -o $@ -v2 -r44100 -n8000 -a0.05 -d0.05 -l1.0

clean:
	-rm -f *.pipe
	-rm -f *.rank
	-rm -f *.rsf
	-rm -f *.raw *.wav
	-rm -f *.dft *.fis


cleanall: clean

