#include <sys/stat.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rankfile.H"

int main(int argc, char* argv[]) {
  // argv[1] = path/name_fundamental.dft
  // argv[2] = renormalization factor  [1000]
  // argv[4] = samples per second [44100]
  // argv[3] = seconds to repeat [10]
  // output file is path/name_fundamental.raw

  // we assume the dft file is prenormalized to unity

  char *spectrum_fn = argv[1];
  char *raw_fn = (char*)alloca(strlen(spectrum_fn)+1);
  bzero(raw_fn, strlen(spectrum_fn)+1);
  strcpy(raw_fn, spectrum_fn);
  float fundamental = 0.0;
  char *tex = raw_fn + strlen(spectrum_fn) - 4;
  *tex=0;
  char *tus = tex;
  while(*(--tus) != '_'); 
  fundamental = atof(tus+1);
  int renorm = 3000;
  if(argc > 2) {
    renorm = atoi(argv[2]);
  }
  int sample_rate = 44100;
  if(argc > 4) {
    sample_rate = atoi(argv[4]);
  }
  float duration = 10.0;
  if(argc > 3) {
    duration = atof(argv[3]);
  }
  strcpy(tex,".raw");
  struct stat ss;
  stat(spectrum_fn, &ss);

  int drecs = ss.st_size/sizeof(dft_freq_point_t);


  printf("Spectrum file: %s\n",spectrum_fn);
  printf("  DFT records: %d\n",drecs);
  printf("     Raw file: %s\n",raw_fn);
  printf("  Fundamental: %f\n",fundamental);
  printf("Renorm factor: %d\n",renorm);
  printf("  Sample rate: %d\n",sample_rate);
  printf("     Duration: %f\n",duration);


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

  for(int j=0; j < drecs; ++j) {
    precs[j].freq = dftrecs[j].mFrequency;
    precs[j].mag = sqrt(dftrecs[j].mRealFactor * dftrecs[j].mRealFactor + dftrecs[j].mImagFactor * dftrecs[j].mImagFactor);
    precs[j].phase = atan2(dftrecs[j].mImagFactor, dftrecs[j].mRealFactor);
  }

  FILE* rf = fopen(raw_fn, "wb");

  int total_samples = (int)(duration * (float)sample_rate);
  for(int i=0; i< total_samples; ++i) {
    float cur_val = 0.0;
    float cur_time = (float)i/(float)sample_rate;

    for(int j=0;j<drecs;++j) {
      cur_val += precs[j].mag * sin(2.0*M_PI*precs[j].freq*cur_time + precs[j].phase);
    }

    cur_val *= renorm;
    int16_t rnd_val = (int16_t)cur_val;
    fwrite(&rnd_val, sizeof(int16_t), 1, rf);
  }

  fclose(rf);

  delete []  precs;
  delete [] dftrecs;

  return 0;
}
