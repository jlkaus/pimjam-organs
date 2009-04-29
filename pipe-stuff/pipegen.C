#include <iostream>
#include <string>
#include <sys/stat.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <argp.h>    /* arg_parse and co. */
#include "rankfile.H"



const char *argp_program_version = "pipegen 0.5";
const char *argp_program_bug_address = "<jlkaus@gmail.com>";
static char program_documentation[] = "pipegen - Program to take a spectrum file and generate a pipe file.\n";


static struct argp_option program_options[] = {
  {"output",	'o',"FILE",	0,"Specifies the output pipe filename.  The name of the file is used to determine the frequency of the pipe to generate."},
  {"freq",	'f',"FREQ",	0,"Generate the output at frequency FREQ.  If not specified, the frequency is determined by the output filename."},
  {"rate",	'r',"RATE",	0,"The sample rate for the output. 44100 is the default."},
  {"norm",	'n',"NORM",	0,"The audio file max amplitude is normalized to this value.  1.0 is the default."},
  {"length",	'l',"LENGTH",	0,"How long the resulting pipe sample should hold for, in seconds.  Default is 1 second.  The system will probably round up to a integer number of wavelengths for repeats."},
  {"verbosity",	'v',"VERBOSITY",	0,"Use VERBOSITY as the level of verbosity while generating the output file."},
  {0}
};

struct program_arguments {
  char *input;
  char *output;
  float freq;
  int rate;
  float norm;
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
    args->norm = atof(arg);
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
  struct program_arguments args = {NULL, NULL, 0.0, 44100, 1.0, 1.0, 0};
  argp_parse(&program_arg_parser, argc, argv, 0,0, &args);

  bool isDft = false;
  float fundamental = args.freq;
  float sourcefreq = 1.0;
  float renorm = args.norm;
  int sample_rate = args.rate;
  float duration = args.length;

  // we assume the dft file is prenormalized to unity

  char *spectrum_fn = args.input;
  char *pipe_fn = args.output;

  if(!spectrum_fn && !pipe_fn) {
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
      for(;dex > pipe_fn-1 && *dex != '_'; --dex);
      sourcefreq = atof(dex+1);
    }
  }

  if(pipe_fn) {
    // figure out, if needed, what the output frequency should be
    char* dex = pipe_fn + strlen(pipe_fn);

    for(;dex > pipe_fn-1 && *dex != '.'; --dex);

    if(fundamental == 0.0) {
      for(;dex > pipe_fn-1 && *dex != '_'; --dex);
      fundamental = atof(dex+1);
    }
  }

  if(spectrum_fn && !pipe_fn) {
    // we have an input filename, but no output filename, so we'll have to copy it over.
    pipe_fn = (char*)alloca(strlen(spectrum_fn)+2);
    bzero(pipe_fn, strlen(spectrum_fn)+2);
    strcpy(pipe_fn, spectrum_fn);
    char *tex = pipe_fn + strlen(spectrum_fn) - 4;
    *tex=0;
    strcpy(tex,".pipe");
  } else if(!spectrum_fn && pipe_fn) {
    // we have no input filename, so we'll have to copy it over.  Assume fis.
     spectrum_fn = (char*)alloca(strlen(pipe_fn)+1);
     bzero(spectrum_fn, strlen(pipe_fn)+1);
     strcpy(spectrum_fn, pipe_fn);
     char *tex = spectrum_fn + strlen(pipe_fn) - 5;
     for(;tex > spectrum_fn-1 && *tex != '_'; --tex);
     *tex=0;
     strcpy(tex,".fis");
  }

  struct stat ss;
   stat(spectrum_fn, &ss);

  int drecs = ss.st_size/sizeof(dft_freq_point_t);
   
  if(args.verbosity) {
    printf("Spectrum file: %s\n",spectrum_fn);
    printf("  DFT records: %d\n",drecs);
    printf("    Pipe file: %s\n",pipe_fn);
    printf("  Fundamental: %f\n",fundamental);
    printf("Renorm factor: %f\n",renorm);
    printf("  Sample rate: %d\n",sample_rate);
    printf("     Duration: %f\n",duration);
  }

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

  // try to come up with total samples.  lower bound is duration * sample_rate, but then up that to get an even multiple of the period.
  // duration (s) * lowest frequency (hz) = number of periods in duration.  Then round that up, divide by frequency again, and use that as the duration.  I think.
  int total_samples = (int)(ceil(duration*precs[0].freq)/precs[0].freq * (float)sample_rate);
  float new_duration = (float)total_samples/(float)sample_rate;

  if(args.verbosity) {
    printf("Total samples: %d\n",total_samples);
    printf(" New duration: %f\n",new_duration);
  }
 
  FILE* rf = fopen(pipe_fn, "wb");

  pipe_hdr_t pipeHeader;

  pipeHeader.mFundamental = fundamental;   // cycles per second
  pipeHeader.mSustainDuration = new_duration;  // seconds in pipe file
  pipeHeader.mRsvd1 = 0.0;
  pipeHeader.mRsvd2 = 0.0;

  pipeHeader.mRelativeVolume = 1.0;
  pipeHeader.mDecayRate = 0.0;
  pipeHeader.mRsvd3 = 0.0;
  pipeHeader.mRsvd4 = 0.0;

  pipeHeader.mSustainSamples = total_samples;  // samples in pipe file
  pipeHeader.mRsvd5 = -1;
  pipeHeader.mRsvd6 = -1;
  pipeHeader.mRsvd7 = -1;

  int temp = -1;
  memcpy(&pipeHeader.mRsvd1,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd2,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd3,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd4,&temp,sizeof(int));

  fwrite(&pipeHeader,sizeof(pipe_hdr_t),1,rf);

  for(int i=0; i< total_samples; ++i) {
    float cur_val = 0.0;
    float cur_time = (float)i/(float)sample_rate;
    float rel_level = 1.0;

    for(int j=0;j<drecs;++j) {
      if(precs[j].freq < sample_rate/2) {
	cur_val += precs[j].mag * sin(2.0*M_PI*precs[j].freq*cur_time + precs[j].phase);
      }
    }

    cur_val *= rel_level/power * renorm;
    fwrite(&cur_val, sizeof(float), 1, rf);
  }

  fclose(rf);

  if(args.verbosity) {
    printf("Wrote out %d samples.\n",total_samples);
  }

  delete []  precs;
  delete [] dftrecs;

  return 0;
}

