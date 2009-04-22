#include <sys/stat.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <argp.h>    /* arg_parse and co. */
#include "rankfile.H"

const char *argp_program_version = "dftwav 0.5";
const char *argp_program_bug_address = "<jlkaus@gmail.com>";
static char program_documentation[] = "dftwav - Program to take a spectrum file and generate an audio file, either raw or wav.\n";


static struct argp_option program_options[] = {
  {"output",	'o',"FILE",	0,"Specifies the output audio filename.  The name of the file is used to determine what type of audio file to generate, and possibly the frequency."},
  {"freq",	'f',"FREQ",	0,"Generate the output at frequency FREQ.  If not specified, the frequency is determined by the output filename."},
  {"rate",	'r',"RATE",	0,"The sample rate for the output. 44100 is the default."},
  {"norm",	'n',"NORM",	0,"The audio file max amplitude is normalized to this value, out of 32000.  8000 is the default."},
  {"attack",	'a',"ATTACK",	0,"Attack time, in decimal seconds.  Default is 0.05 seconds."},
  {"decay",	'd',"DECAY",	0,"Decay time, in decimal seconds.  Default is 0.05 seconds."},
  {"length",	'l',"LENGTH",	0,"How long the resulting sample should play for, in seconds.  Default is 1 second."},
  {"verbosity",	'v',"VERBOSITY",	0,"Use VERBOSITY as the level of verbosity while generating the output file."},
  {0}
};

struct program_arguments {
  char *input;
  char *output;
  float freq;
  int rate;
  int norm;
  float attack;
  float decay;
  float length;
  int verbosity;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct program_arguments *args = (program_arguments*)state->input;

  switch(key) {
  case ARGP_KEY_ARG:
    args->input = arg;
    break;
  case 'o':
    args->output = arg;
    break;
  case 'f':
    args->freq = atof(arg);
    break;
  case 'r':
    args->rate = atoi(arg);
    break;
  case 'n':
    args->norm = atoi(arg);
    break;
  case 'a':
    args->attack = atof(arg);
    break;
  case 'd':
    args->decay = atof(arg);
    break;
  case 'l':
    args->length = atof(arg);
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


int main(int argc, char* argv[]) {
  struct program_arguments args = {NULL, NULL, 0.0, 44100, 8000, 0.05, 0.05, 1.0, 0};
  argp_parse(&program_arg_parser, argc, argv, 0,0, &args);

  bool isRaw = false;
  bool isDft = false;
  float fundamental = args.freq;
  float sourcefreq = 1.0;
  int renorm = args.norm;
  int sample_rate = args.rate;
  float duration = args.length;
  float attack_t = args.attack;
  float decay_t = args.decay;

  // we assume the dft file is prenormalized to unity

  char *spectrum_fn = args.input;
  char *raw_fn = args.output;

  if(!spectrum_fn && !raw_fn) {
    fprintf(stderr, "No input or output filename specified. Aborting.\n");
    exit(-1);
  }

  if(spectrum_fn) {
    // figure out what type of file the input is
    char* dex = spectrum_fn + strlen(spectrum_fn);

    for(;dex > spectrum_fn-1 && *dex != '.'; --dex);
    if(strcmp(dex+1,"dft")==0) {
      isDft = true;

      // we have a dft file, so we have to renormalize the frequencies of the dft on the fly
      for(;dex > raw_fn-1 && *dex != '_'; --dex);
      sourcefreq = atof(dex+1);
    }
  }

  if(raw_fn) {
    // figure out what type of file the output is
    // figure out, if needed, what the output frequency should be
    char* dex = raw_fn + strlen(raw_fn);

    for(;dex > raw_fn-1 && *dex != '.'; --dex);
    if(strcmp(dex+1,"raw")==0) {
      isRaw = true;
    }

    if(fundamental == 0.0) {
      for(;dex > raw_fn-1 && *dex != '_'; --dex);
      fundamental = atof(dex+1);
    }
  }

  if(spectrum_fn && !raw_fn) {
    // we have an input filename, but no output filename, so we'll have to copy it over.  Assume wav.
    raw_fn = (char*)alloca(strlen(spectrum_fn)+1);
    bzero(raw_fn, strlen(spectrum_fn)+1);
    strcpy(raw_fn, spectrum_fn);
    char *tex = raw_fn + strlen(spectrum_fn) - 4;
    *tex=0;
    strcpy(tex,".wav");
  } else if(!spectrum_fn && raw_fn) {
    // we have no input filename, so we'll have to copy it over.  Assume fis.
    spectrum_fn = (char*)alloca(strlen(raw_fn)+1);
    bzero(spectrum_fn, strlen(raw_fn)+1);
    strcpy(spectrum_fn, raw_fn);
    char *tex = spectrum_fn + strlen(raw_fn) - 4;
    *tex=0;
    strcpy(tex,".fis");
  }



  struct stat ss;
  stat(spectrum_fn, &ss);

  int drecs = ss.st_size/sizeof(dft_freq_point_t);

  if(args.verbosity) {
    printf("Spectrum file: %s\n",spectrum_fn);
    printf("  DFT records: %d\n",drecs);
    printf("     Raw file: %s\n",raw_fn);
    printf("  Fundamental: %f\n",fundamental);
    printf("Renorm factor: %d\n",renorm);
    printf("  Sample rate: %d\n",sample_rate);
    printf("     Duration: %f\n",duration);
    printf("  Attack time: %f\n",attack_t);
    printf("   Decay time: %f\n",decay_t);
  }

  float sustain_t = attack_t;
  float attack_constant = -log(0.01)/attack_t;
  float release_t = duration-decay_t;
  float decay_constant = -log(0.01)/decay_t;

  dft_freq_point_t* dftrecs = new dft_freq_point_t[drecs];

  FILE* sf = fopen(spectrum_fn, "rb");
  fread(dftrecs, sizeof(dft_freq_point_t), drecs, sf);
  fclose(sf);

  struct polar_form {
    float freq;
    float mag;
    float phase;
  };

  polar_form* precs = new polar_form[drecs];

  float power = 0.0;
  for(int j=0; j < drecs; ++j) {
    precs[j].freq = dftrecs[j].mFrequency*fundamental/sourcefreq;
    precs[j].mag = sqrt(dftrecs[j].mRealFactor * dftrecs[j].mRealFactor + dftrecs[j].mImagFactor * dftrecs[j].mImagFactor);
    if(precs[j].freq < sample_rate/2) {
      power += precs[j].mag;
    }
    precs[j].phase = atan2(dftrecs[j].mImagFactor, dftrecs[j].mRealFactor);
  }

  if(args.verbosity) {
    printf("Spectral Power: %f\n",power);
  }

  FILE* rf = fopen(raw_fn, "wb");

  int total_samples = (int)(duration * (float)sample_rate);
  for(int i=0; i< total_samples; ++i) {
    float cur_val = 0.0;
    float cur_time = (float)i/(float)sample_rate;
    float rel_level = 1.0;
    if(cur_time < sustain_t) {
      rel_level = 1.0 - 1.0*exp(-attack_constant * cur_time);
    } else if(cur_time > release_t) {
      rel_level = 1.0 * exp(-decay_constant * (cur_time - release_t)); 
    }

    for(int j=0;j<drecs;++j) {
      if(precs[j].freq < sample_rate/2) {
	cur_val += precs[j].mag * sin(2.0*M_PI*precs[j].freq*cur_time + precs[j].phase);
      }
    }

    cur_val *= rel_level/power * renorm;
    int16_t rnd_val = (int16_t)cur_val;
    fwrite(&rnd_val, sizeof(int16_t), 1, rf);
  }

  fclose(rf);

  if(args.verbosity) {
    printf("Wrote out %d samples.\n",total_samples);
  }

  delete []  precs;
  delete [] dftrecs;

  return 0;
}
