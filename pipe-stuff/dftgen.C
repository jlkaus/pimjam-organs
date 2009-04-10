#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <argp.h>    /* arg_parse and co. */
#include "rankfile.H"

const char *argp_program_version = "dftgen 0.5";
const char *argp_program_bug_address = "<jlkaus@gmail.com>";
static char program_documentation[] = "dftgen - Program to generate some theoretical pipe spectrums.\n";


static struct argp_option program_options[] = {
  {"output",	'o',"PATH/XxFxVxSxPb_Ll[_f.dft|.fis]",	0,"Specifies the output spectrum file.  The name of the file is used to determine what type of pipe to generate."},
  {"freq",	'f',"FREQ",	0,"Generate a dft file at frequency FREQ.  If not specified, a fis file is generated, or a dft with the frequency determined by the filename."},
  {"verbosity",	'v',"VERBOSITY",	0,"Use VERBOSITY as the level of verbosity while generating the pipe spectrum."},
  {0}
};

struct program_arguments {
  char *output;
  float freq;
  int verbosity;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct program_arguments *args = state->input;

  switch(key) {

  case 'o':
    args->output = arg;
    break;
  case 'f':
    args->freq = atof(arg);
    break;
  case 'v':
    args->verbosity = atoi(arg);
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

static struct argp program_arg_parser = {program_options, parse_opt, 0, program_documentation};

void generateDftv(dft_freq_point_t*, float, float, float, float, bool, int, float, int);

int main(int argc, char* argv[]) {
  // generate only one pipe
  // input parm is just a directory/filename.  Filename is parsed to determine pipe characteristics and frequency.  Directory is just used to place the dft into.
  //  path/Xx.xxFx.xxVx.xxSx.xxPx_Ll_ffff.dft
  // verbosity is argv[2]

  struct program_arguments args = {"", 0.0, 0};
  argp_parse(&program_arg_parser, argc, argv, 0,0, &args);

  char* dir_name=args->output;
  char* spectrum_fn;
  char sysstuff[512];
  bzero(sysstuff,512);

  int verbosity = args->verbosity;

  float Dx=0.0;
  float Af=0.0;
  float Ae=0.0;
  float As=0.0;
  bool phasing = false;
  int limitation=0;
  float fundamental = args->freq;

  char* dex = argv[1] + strlen(argv[1]);

  for(;dex > dir_name-1 && *dex != '.'; --dex);
  if(strcmp(dex+1,"dft")==0) {
    if(fundamental == 0.0) {
      for(;dex > dir_name-1 && *dex != '_'; --dex);
      fundamental = atof(dex+1);      
    }
  } else {
    fundamental = 1.0;
  }

  if(fundamental == 0.0) {
    fprintf(stderr,"Trying to generate a .dft file with no frequency specified with --freq or in the filename. Aborting.\n");
    exit(-1);
  }

  for(;dex > dir_name-1 && *dex != 'L'; --dex);
  limitation = atoi(dex+1);

  for(;dex > dir_name-1 && *dex != 'P'; --dex);
  phasing = atoi(dex+1);

  for(;dex > dir_name-1 && *dex != 'S'; --dex);
  As = atof(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'V'; --dex);
  Ae = atof(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'F'; --dex);
  Af = atof(dex+1) / 100.0;

  for(;dex > dir_name-1 && *dex != 'X'; --dex);
  Dx = atof(dex+1) / 100.0;

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
    printf("  Dx %f\n", Dx);
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
      generateDftv(&dftv, Dx, Af, Ae, As, phasing, limitation, fundamental, k);
      if(fabs(dftv.mRealFactor) > 1e-14 || fabs(dftv.mImagFactor) > 1e-14) {
	norm += sqrt(dftv.mRealFactor * dftv.mRealFactor + dftv.mImagFactor * dftv.mImagFactor);
      }
    }

    // Now that we have our normalization, actual generate and write the harmonics to the output file
    for(int k = 1; k <= (int)k_max; ++k) {
      dft_freq_point_t dftv;
      generateDftv(&dftv, Dx, Af, Ae, As, phasing, limitation, fundamental, k);
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


void generateDftv(dft_freq_point_t* dftv, float Dx, float Af, float Ae, float As, bool phasing, int limitation, float fundamental, int k) {
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
