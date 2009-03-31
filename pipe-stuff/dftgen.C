#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rankfile.H"

void generateDftv(dft_freq_point_t*, float, int, int, int, int, int, float, int, float, float, float);

int main(int argc, char* argv[]) {
  // generate all dfts.
  // parms:	max mutation harmonic (9)
  //		detuning amount in cents (2)
  //		note range, in octave numbers (0-9)
  //		imperfection of stopping (0.25)
  //            imperfection of overblowing (0.25)
  //		decay rate step (0.50)
  //		decay rate max (2.00)
  //            cutoff frequency (22050)
  //		limitation imperfection (0.25)
  //		list of limitation harmonics to generate other than fake ones for L0p and L0i

  // will place all dfts into directories based on pipe family (about 90 directories, given defaults. decay rate parameter changes might generate more): Xddxxyyz
  // So each directoy will contain all dfts for that family, including all colors, all mutations, all notes, and all tunings.  Given defaults, about 20,000 dfts per directory.

  int max_mut_harm = 9;
  if(argc > 1) {
    max_mut_harm = atoi(argv[1]);
  }
  int detune = 2;
  if(argc > 2) {
    detune = atoi(argv[2]);
  }
  int oct_low = 0;
  if(argc > 3) {
    oct_low = atoi(argv[3]);
  }
  int oct_high = 9;
  if(argc > 4) {
    oct_high = atoi(argv[4]);
  }
  float imperfect_stop = 0.25;
  if(argc > 5) {
    imperfect_stop = atof(argv[5]);
  }
  float imperfect_overblow = 0.25;
  if(argc > 6) {
    imperfect_overblow = atof(argv[6]);
  }
  float decay_step = 0.50;
  if(argc > 7) {
    decay_step = atof(argv[7]);
  }
  float decay_max = 2.00;
  if(argc > 8) {
    decay_max = atof(argv[8]);
  }
  float f_max = 22050.0;
  if(argc > 9) {
    f_max = atof(argv[9]);
  }
  float imperfect_limitation = 0.25;
  if(argc > 10) {
    imperfect_limitation = atof(argv[10]);
  }

  char dir_name[256];
  char spectrum_fn[256];

  // decay modes: 0.00:decay_step:decay_max
  for(float decay = 0.00; decay <= decay_max; decay+=decay_step) {
    bzero(dir_name, 256);
    bzero(spectrum_fn, 256);
    // add decay portion to dir and fn

    // stoppage: 0:U, 1:Si, 2:Sp
    for(int stoppage = 0; stoppage < 3; ++stoppage) {


      // overblow: 0:B, 1:Oi, 2:Op
      for(int overblow = 0; overblow < 3; ++overblow) {


	// phasing:  0:C, 1:A
	for(int phasing = 0; phasing < 2; ++phasing) {


	  // directories created at this level


	  // limitation ranging: L0p, L0i, argv[11+]
	  for(int lim_index = 0; lim_index < argc - 11; ++lim_index) {
	    for(int lim_imperfect = 0; lim_imperfect < 2; ++lim_imperfect) {


	      // octaves: oct_low:oct_high
	      for(int octave = oct_low; octave < oct_high; ++octave) {
		float base_freq = 440.0 * pow(2.0, (-57.0+12.0*octave)/12.0)
		// notes: C, C#, D, D#, E, F, F#, G, G#, A, A#, B
		for(float note_cents = 0.00; note_cents < 1200.00; note_cents+=100.00) {
		  // mutations: M1, M2, M3, M4, M5, M6, M7, M8, M9
		  for(int mutation = 1; mutation <= max_mut_harm; ++mutation) {


		    float mutation_cents = (1200.00/M_LN2) * log((float)mutation);
		    // detunings: under, perfect, over
		    for(int detune_c = -detune; detune_c <= detune; detune_c += detune) {


		      float detune_cents = (float)detune_c;
		      flaot f_cents = note_cents + mutation_cents + detune_cents;
		      float fundamental = base_freq * pow(2.0, f_cents);



		      // determine highest harmonic we can use
		      float k_max = log(f_max / fundamental)/M_LN2;
		      
		      FILE* sf = fopen(spectrum_fn, "wb");

		      float norm = 0.0;
		      // determine normalization by going through things once
		      for(int k = 1; k <= (int)k_max; ++k) {
			dft_freq_point_t dftv;
			generateDftv(&dftv, decay, stoppage, overblow, phasing, ((lim_index == 0)?0:atoi(argv[lim_index+10])), lim_imperfect, fundamental, k, imperfect_stop, imperfect_overblow, imperfect_limitation);
			norm += sqrt(dftv.mRealFactor * dftv.mRealFactor + dftv.mImagFactor * dftv.mImagFactor);
		      }

		      // Now that we have our normalization, actual generate and write the harmonics to the output file
		      for(int k = 1; k <= (int)k_max; ++k) {
			dft_freq_point_t dftv;
			generateDftv(&dftv, decay, stoppage, overblow, phasing, ((lim_index == 0)?0:atoi(argv[lim_index+10])), lim_imperfect, fundamental, k, imperfect_stop, imperfect_overblow, imperfect_limitation);
			dftv.mRealFactor/=norm;
			dftv.mImagFactor/=norm;
			fwrite(&dftv, sizeof(dft_freq_point_t), 1, sf);
		      }

		      fclose(sf);
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }

  return 0;
}


void generateDftv(dft_freq_point_t* dftv, float decay, int stoppage, int overblow, int phasing, int lim_cutoff, int lim_imperfect, float fundamental, int k, float imperfect_stop, float imperfect_overblow, float imperfect_limitation) {
  bzero(dftv, sizeof(dft_freq_point_t));



}
