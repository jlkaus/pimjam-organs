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
  char color_dir[256];
  char sysspace[256];

  // decay modes: 0.00:decay_step:decay_max
  for(float decay = 0.00; decay <= decay_max; decay+=decay_step) {
    bzero(dir_name, 256);
    // add decay portion to dir and fn
    int dm = (int)(decay * 10);
    sprintf(dir_name,"X%02d",dm);

    // stoppage: 0:UU, 1:Si, 2:Sp
    for(int stoppage = 0; stoppage < 3; ++stoppage) {
      if(stoppage == 0) {
	strcpy(dir_name+3, "UU");
      } else if(stoppage == 1) {
	strcpy(dir_name+3, "Si");
      } else {
	strcpy(dir_name+3, "Sp");
      }

      // overblow: 0:BB, 1:Oi, 2:Op
      for(int overblow = 0; overblow < 3; ++overblow) {
	if(overblow == 0) {
	  strcpy(dir_name+5, "BB");
	} else if(overblow == 1) {
	  strcpy(dir_name+5, "Oi");
	} else {
	  strcpy(dir_name+5, "Op");
	}

	// phasing:  0:C, 1:A
	for(int phasing = 0; phasing < 2; ++phasing) {
	  if(phasing == 0) {
	    strcpy(dir_name+7,"C");
	  } else {
	    strcpy(dir_name+7,"A");
	  }

	  printf("Creating pipe family %s...\n",dir_name);

	  // limitation ranging: L00p, L00i, argv[11+]
	  for(int lim_index = 0; lim_index <= (argc >=11)?(argc - 11):0; ++lim_index) {
	    for(int lim_imperfect = 0; lim_imperfect < 2; ++lim_imperfect) {
	      bzero(color_dir,256);
	      int lim_cutoff = ((lim_index == 0)?0:atoi(argv[lim_index+10]));
	      sprintf(color_dir,"L%02d%c",lim_cutoff, lim_imperfect?'i':'p');

	      // directories created at this level
	      bzero(sysspace,256);
	      strcat(sysspace, "mkdir -p THP_");
	      strcat(sysspace, dir_name);
	      strcat(sysspace, "/");
	      strcat(sysspace, color_dir);
	      system(sysspace);

	      printf("  Creating color group %s...\n",color_dir);

	      // octaves: oct_low:oct_high
	      for(int octave = oct_low; octave < oct_high; ++octave) {
		float base_freq = 440.0 * pow(2.0, (-57.0+12.0*octave)/12.0);
		// notes: C, C#, D, D#, E, F, F#, G, G#, A, A#, B
		for(float note_cents = 0.00; note_cents < 1200.00; note_cents+=100.00) {
		  // mutations: M1, M2, M3, M4, M5, M6, M7, M8, M9
		  for(int mutation = 1; mutation <= max_mut_harm; ++mutation) {
		    bzero(spectrum_fn,256);
		    sprintf(spectrum_fn,"_M%01d",mutation);

		    float mutation_cents = (1200.00/M_LN2) * log((float)mutation);
		    // detunings: under, perfect, over
		    for(int detune_c = -detune; detune_c <= detune; detune_c += detune) {
		      spectrum_fn[3] = (detune_c < 0)?'u':((detune_c==0)?'p':'o');

		      float detune_cents = (float)detune_c;
		      float f_cents = note_cents + mutation_cents + detune_cents;
		      float fundamental = base_freq * pow(2.0, f_cents/1200.00);

		      sprintf(spectrum_fn+4,"_%.3f",fundamental);

		      // determine highest harmonic we can use
		      float k_max = f_max / fundamental;

		      bzero(sysspace,256);
		      strcat(sysspace, "THP_");
		      strcat(sysspace, dir_name);
		      strcat(sysspace,"/");
		      strcat(sysspace,color_dir);
		      strcat(sysspace,"/");
		      strcat(sysspace, dir_name);
		      strcat(sysspace,color_dir);
		      strcat(sysspace,spectrum_fn);
		      strcat(sysspace,".dft");	      

		      printf("O %1d, BF %.3f, NC %06.2f, M %1d, MC %06.2f, ", octave, base_freq, note_cents, mutation, mutation_cents);
		      printf("D %d, DC %06.2f, FC %06.2f, F %.3f, K %d, ", detune_c, detune_cents, f_cents, fundamental, (int)k_max);
		      printf("%s\n", spectrum_fn);

		      FILE* sf = fopen(sysspace, "wb");

		      float norm = 0.0;
		      // determine normalization by going through things once
		      for(int k = 1; k <= (int)k_max; ++k) {
			dft_freq_point_t dftv;
			generateDftv(&dftv, decay, stoppage, overblow, phasing, lim_cutoff, lim_imperfect, fundamental, k, imperfect_stop, imperfect_overblow, imperfect_limitation);
			norm += sqrt(dftv.mRealFactor * dftv.mRealFactor + dftv.mImagFactor * dftv.mImagFactor);
		      }

		      // Now that we have our normalization, actual generate and write the harmonics to the output file
		      for(int k = 1; k <= (int)k_max; ++k) {
			dft_freq_point_t dftv;
			generateDftv(&dftv, decay, stoppage, overblow, phasing, lim_cutoff, lim_imperfect, fundamental, k, imperfect_stop, imperfect_overblow, imperfect_limitation);
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
  dftv->mFrequency = fundamental * (float)k;

  // deal with decay
  float initial_strength = pow((float)k, -decay);

  // deal with overblowing
  if(overblow == 0) {
    // normal blow, full strength fundamental
  } else if(overblow == 1) {
    // reduced fundamental
    initial_strength *= imperfect_overblow;
  } else {
    // no fundamental
    initial_strength = 0.0;
  }

  // deal with stopped pipes on even harmonics
  if(k % 2 == 0) {
    // even harmonic
    if(stoppage == 0) {
      // no stoppage, full strength evens
    } else if(stoppage == 1) {
      // reduced evens
      initial_strength *= imperfect_stop;
    } else {
      // no evens
      initial_strength = 0.0;
    }
  }

  // deal with cutoff frequencies
  if(lim_cutoff == 0) {
    if(lim_imperfect == 0) {
      // fake limiter.  only fundamental is present.
      if(k>1) {
	initial_strength = 0.0;
      }
    } else {
      // fake limiter. No limitation.
    }
  } else {
    // real limiting
    if(k == lim_cutoff-1) {
      if(lim_imperfect) {
	initial_strength *= (1-imperfect_limitation)*0.85+imperfect_limitation;
      } else {
	initial_strength *= 0.85;
      }
    } else if(k == lim_cutoff) {
      if(lim_imperfect) {
	initial_strength *= (1-imperfect_limitation)*0.5+imperfect_limitation;
      } else {
	initial_strength *= 0.5;
      }
    } else if(k == lim_cutoff+1) {
      if(lim_imperfect) {
	initial_strength = (1-imperfect_limitation)*0.15+imperfect_limitation;
      } else {
	initial_strength *= 0.15;
      }
    } else if(k > lim_cutoff+1) {
      if(lim_imperfect) {
	initial_strength = imperfect_limitation;
      } else {
	initial_strength = 0.0;
      }
    }
  }


  // deal with phasing
  if(phasing) {
    // phasing causes rotation
    if(k % 4 == 1) {
      dftv->mRealFactor = initial_strength;
    } else if (k%4 == 2) {
      dftv->mImagFactor = initial_strength;
    } else if(k%4 == 3) {
      dftv->mRealFactor = -initial_strength;
    } else {
      dftv->mImagFactor = -initial_strength;
    }
  } else {
    // no rotation. how boring...
    dftv->mRealFactor = initial_strength;
  }
}
