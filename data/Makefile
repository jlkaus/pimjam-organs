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

testing-raws: testing/X1.00F1.00V0.05S0.05P0_L20_261.626.raw \
	testing/X1.00F1.00V0.05S0.05P0_L20_293.665.raw\
	testing/X1.00F1.00V0.05S0.05P0_L20_329.628.raw\
	testing/X1.00F1.00V0.05S0.05P0_L20_349.228.raw\
	testing/X1.00F1.00V0.05S0.05P0_L20_391.995.raw\
	testing/X1.00F1.00V0.05S0.05P0_L20_440.000.raw\
	testing/X1.00F1.00V0.05S0.05P0_L20_493.884.raw\
	testing/X1.00F1.00V0.05S0.05P0_L20_523.251.raw



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
	../scripts/ssffis.pl $< -o $@ -fauto
%.fis: %.wav ../bin/wavdft
	../bin/wavdft $< -o $@ -fauto
%.fis: ../bin/dftgen
	../bin/dftgen -o $@ -v2
%.dft: ../bin/dftgen
	../bin/dftgen -o $@ -v2
%.raw: %.dft ../bin/dftwav
	../bin/dftwav $< -o $@ -r44100 -n8000 -a0.05 -d0.05 -l1.0
%.wav: %.dft ../bin/dftwav
	../bin/dftwav $< -o $@ -r44100 -n8000 -a0.05 -d0.05 -l1.0
%.pipe: %.dft ../bin/pipegen
	../bin/pipegen $< -o $@ -r44100 -n1.0 -l1.0
%.rank: %.rsf ../bin/rankgen
	-include %.rsf
	../bin/rankgen $< -o $@

clean:
	-rm -f *.pipe
	-rm -f *.rank
	-rm -f *.rsf
	-rm -f *.raw *.wav
	-rm -f *.dft *.fis


cleanall: clean

