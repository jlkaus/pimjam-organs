#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rankfile.H"




int main(int argc, char* argv[]) {
  // argv[1] = path/name_fundamental.dft
  // argv[2] = renormalization factor  [1.0]
  // argv[3] = seconds to repeat [5]
  // argv[4] = seconds for attack [0.05]
  // argv[5] = overshoot in attack [1.1]
  // argv[6] = samples per second [44100]
  // output file is path/name_fundamental.raw

  // we assume the dft file is prenormalized to unity

  char *spectrum_fn = argv[1];
  char *pipe_fn = (char*)alloca(strlen(spectrum_fn)+2);
  bzero(pipe_fn, strlen(spectrum_fn)+2);
  strcpy(pipe_fn, spectrum_fn);
  float fundamental = 0.0;
  char *tex = pipe_fn + strlen(spectrum_fn) - 4;
  *tex=0;
  char *tus = tex;
  while(*(--tus) != '_'); 
  fundamental = atof(tus+1);
  float renorm = 1.0;
  if(argc > 2) {
    renorm = atof(argv[2]);
  }
  int sample_rate = 44100;
  if(argc > 6) {
    sample_rate = atoi(argv[6]);
  }
  float duration = 5.0;
  if(argc > 3) {
    duration = atof(argv[3]);
  }
  float attack_t = 0.05;
  if(argc > 4) {
    attack_t = atof(argv[4]);
  }
  float overshoot = 1.1;
  if(argc > 5) {
    overshoot = atof(argv[5]);
  }
  strcpy(tex,".pipe");
  struct stat ss;
  stat(spectrum_fn, &ss);

  int drecs = ss.st_size/sizeof(dft_freq_point_t);


  printf("Spectrum file: %s\n",spectrum_fn);
  printf("  DFT records: %d\n",drecs);
  printf("    Pipe file: %s\n",pipe_fn);
  printf("  Fundamental: %f\n",fundamental);
  printf("Renorm factor: %f\n",renorm);
  printf("  Sample rate: %d\n",sample_rate);
  printf("     Duration: %f\n",duration);
  printf("  Attack time: %f\n",attack_t);
  printf("    Overshoot: %f\n",overshoot);

  // Note: for first 3/4 attack time, we ramp from 0.0 to overshoot
  //       for last 1/4 attack time, we decay from overshoot to 1.0
  float ramp_t = attack_t * 3.0/4.0;
  float ramp_rate = (overshoot-0.0)/(ramp_t-0.0);
  float decay_rate = (1.0 - overshoot)/(attack_t - ramp_t);


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

  FILE* rf = fopen(pipe_fn, "wb");


  pipe_hdr_t pipeHeader;

  pipeHeader.mFundamental = fundamental;   // cycles per second
  pipeHeader.mSustainDuration = duration;  // seconds in pipe file
  pipeHeader.mRsvd1 = 0.0;
  pipeHeader.mRsvd2 = 0.0;

  pipeHeader.mRelativeVolume = 1.0;
  pipeHeader.mDecayRate = 0.0;
  pipeHeader.mRsvd3 = 0.0;
  pipeHeader.mRsvd4 = 0.0;

  pipeHeader.mSustainSamples = (int)(duration * (float)sample_rate);  // samples in pipe file
  pipeHeader.mRsvd5 = -1;
  pipeHeader.mRsvd6 = -1;
  pipeHeader.mRsvd7 = -1;


  int temp = -1;
  memcpy(&pipeHeader.mRsvd1,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd2,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd3,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd4,&temp,sizeof(int));

  fwrite(&pipeHeader,sizeof(pipe_hdr_t),1,rf);

  int total_samples = pipeHeader.mSustainSamples;
  for(int i=0; i< total_samples; ++i) {
    float cur_val = 0.0;
    float cur_time = (float)i/(float)sample_rate;
    float rel_level = 1.0;
    if(cur_time < ramp_t) {
      rel_level = ramp_rate*cur_time;
    } else if(cur_time < attack_t) {
      rel_level = overshoot + decay_rate*(cur_time-ramp_t); 
    }

    for(int j=0;j<drecs;++j) {
      cur_val += precs[j].mag * sin(2.0*M_PI*precs[j].freq*cur_time + precs[j].phase);
    }

    cur_val *= rel_level * renorm;

    fwrite(&cur_val, sizeof(float), 1, rf);
  }

  fclose(rf);

  delete []  precs;
  delete [] dftrecs;

  return 0;
}
