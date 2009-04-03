#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rankfile.H"

void generateDftv(dft_freq_point_t*, float, float, float, float, float, float, bool, int, float, int);

int main(int argc, char* argv[]) {
  // generate only one pipe
  // input parm is just a directory/filename.  Filename is parsed to determine pipe characteristics and frequency.  Directory is just used to place the dft into.
  //  path/XDxxxODxxxEDxxxFxxxExxxSxxxPx_Lllll_ffff.dft
  // verbosity is argv[2]

  if(argc == 1) {
    fprintf(stderr,"Syntax: dftgen [path/]XDxxxODxxxEDxxxFxxxExxxSxxxPb_Lllll_ffff.dft [verbosity]\n");
    exit(-1);
  }

  char* dir_name=argv[1];
  char* spectrum_fn;
  char sysstuff[512];
  bzero(sysstuff,512);

  int verbosity = argc>2 ? atoi(argv[2]) : 0;

  float Dx=0.0;
  float Do=0.0;
  float De=0.0;
  float Af=0.0;
  float Ae=0.0;
  float As=0.0;
  bool phasing = false;
  int limitation=0;
  float fundamental = 0.0;

  char* dex = argv[1] + strlen(argv[1]);

  for(;dex > dir_name-1 && *dex != '_'; --dex);
  fundamental = atof(dex+1);

  for(;dex > dir_name-1 && *dex != 'L'; --dex);
  limitation = atoi(dex+1);

  for(;dex > dir_name-1 && *dex != 'P'; --dex);
  phasing = atoi(dex+1);

  for(;dex > dir_name-1 && *dex != 'S'; --dex);
  As = atoi(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'E'; --dex);
  Ae = atoi(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'F'; --dex);
  Af = atoi(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'D'; --dex);
  De = atoi(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'D'; --dex);
  Do = atoi(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'D'; --dex);
  Dx = atoi(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != '/'; --dex);
  spectrum_fn = dex+1;

  if(*dex == '/') {
    *dex = 0;
    strcat(sysstuff, "mkdir -p ");
    strcat(sysstuff, dir_name);
    system(sysstuff);
    bzero(sysstuff, 512);
    strcat(sysstuff, dir_name);
    strcat(sysstuff, "/");
  }

  strcat(sysstuff, spectrum_fn);

  if(verbosity) {
    printf("Attempting to create %s...\n",sysstuff);
  }

  float k_max = 22050.0 / fundamental;

  if(verbosity > 1) {
    printf("  Dx %f Do %f De %f\n", Dx, Do, De);
    printf("  Af %f Ae %f As %f\n", Af, Ae, As);
    printf("  P %d L %d\n", (int)phasing, limitation);
    printf("  F %f from k = 1 to %d\n", fundamental, (int)k_max);
  }

  int count = 0;

  if(k_max > 1.0) {
    FILE* sf = fopen(sysstuff, "wb");

    float norm = 0.0;
    // determine normalization by going through things once
    for(int k = 1; k <= (int)k_max; ++k) {
      dft_freq_point_t dftv;
      generateDftv(&dftv, Dx, Do, De, Af, Ae, As, phasing, limitation, fundamental, k);
      if(fabs(dftv.mRealFactor) > 1e-14 || fabs(dftv.mImagFactor) > 1e-14) {
	norm += sqrt(dftv.mRealFactor * dftv.mRealFactor + dftv.mImagFactor * dftv.mImagFactor);
      }
    }

    // Now that we have our normalization, actual generate and write the harmonics to the output file
    for(int k = 1; k <= (int)k_max; ++k) {
      dft_freq_point_t dftv;
      generateDftv(&dftv, Dx, Do, De, Af, Ae, As, phasing, limitation, fundamental, k);
      if(fabs(dftv.mRealFactor) > 1e-14 || fabs(dftv.mImagFactor) > 1e-14) {
	dftv.mRealFactor/=norm;
	dftv.mImagFactor/=norm;
	fwrite(&dftv, sizeof(dft_freq_point_t), 1, sf);
	++count;
      }
    }

    fclose(sf);

    if(verbosity) {
      printf("Generated %d non-zero harmonics.\n",count);
    }
  } else if(verbosity) {
    printf("Frequency too high. No harmonics generated.\n");
  }

  return 0;
}


void generateDftv(dft_freq_point_t* dftv, float Dx, float Do, float De, float Af, float Ae, float As, bool phasing, int limitation, float fundamental, int k) {
  dftv->mFrequency = fundamental * (float)k;

  // base magnitude
  float initial_strength = 1.0;
  if(k==0) {
    initial_strength *= Af;
  } else if(k%2 == 0) {
    initial_strength *= Ae;
  }

  // deal with decay
  initial_strength *= pow((float)k, -Dx);
  if(k%2 == 0) {
    initial_strength *= (1.0 - De*(1.0-(float)k));
  } else {
    initial_strength *= (1.0 - Do*(1.0-(float)k));
  }

  // deal with cutoff
  if(k == limitation - 1) {
    initial_strength *= 0.85 + 0.15 * As;
  } else if(k == limitation) {
    initial_strength *= 0.50 + 0.50 * As;
  } else if(k == limitation + 1) {
    initial_strength *= 0.15 + 0.85 * As;
  } else if(k > limitation + 1) {
    initial_strength *= As;
  }

  // deal with phasing
  if(phasing) {
    // phasing causes rotation
    if(k % 4 == 1) {
      dftv->mRealFactor = initial_strength;
      dftv->mImagFactor = 0.0;
    } else if (k%4 == 2) {
      dftv->mRealFactor = 0.0;
      dftv->mImagFactor = initial_strength;
    } else if(k%4 == 3) {
      dftv->mRealFactor = -initial_strength;
      dftv->mImagFactor = 0.0;
    } else {
      dftv->mRealFactor = 0.0;
      dftv->mImagFactor = -initial_strength;
    }
  } else {
    // no rotation. how boring...
    dftv->mRealFactor = initial_strength;
    dftv->mImagFactor = 0.0;
  }
}
