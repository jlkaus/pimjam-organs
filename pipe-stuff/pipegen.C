#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "rankfile.H"




int main(int argc, char* argv[]) {
  // ok, args for pipegen are just the spectrum file to transform.
  // interpret argv[1] to get the rank name and the fundamental
  // then use it to decide what the pipe file name should be

  char *spectrum_fn = argv[1];
  char *pipe_fn = (char*)alloca(strlen(spectrum_fn)+2);
  bzero(pipe_fn, strlen(spectrum_fn)+2);
  strcpy(pipe_fn, spectrum_fn);
  int fundamental = 0;
  char *tex = pipe_fn + strlen(spectrum_fn) - 4;
  *tex=0;
  char *tus = tex;
  while(*(--tus) != '_'); 
  fundamental = atoi(tus+1);
  strcpy(tex,".pipe");

  printf("Spectrum file: %s\n",spectrum_fn);
  printf("Pipe file: %s\n",pipe_fn);
  printf("Fundamental: %d\n",fundamental);

  // create the pipe header structure
  pipe_hdr_t pipeHeader;

  int sample_rate = 44100;  // samples per second

  pipeHeader.mFundamental = fundamental;   // cycles per second
  pipeHeader.mSustainSamples = (int)((float)sample_rate * 2.0/(float)fundamental);  // samples per two cycles
  pipeHeader.mAttackSamples = 0;
  pipeHeader.mReleaseSamples = 0;
  pipeHeader.mSustainDuration = 2.0/(float)fundamental;  // seconds per two cycles
  pipeHeader.mAttackDuration = 0.0;
  pipeHeader.mReleaseDuration = 0.0;
  pipeHeader.mRsvd1 = 0.0;
  pipeHeader.mRelativeVolume = 1.0;
  pipeHeader.mDecayRate = 0.0;
  pipeHeader.mRsvd2 = 0.0;
  pipeHeader.mRsvd3 = 0.0;

  int temp = -1;
  memcpy(&pipeHeader.mRsvd1,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd2,&temp,sizeof(int));
  memcpy(&pipeHeader.mRsvd3,&temp,sizeof(int));


  printf("Sustain Samples: %d\n",pipeHeader.mSustainSamples);
  printf("Sustain Duration: %f\n",pipeHeader.mSustainDuration);

  // open the spectrum file
  //  FILE* spectrum_f = fopen(spectrum_fn,"rb");

  // open and create the pipe file
  FILE* pipe_f = fopen(pipe_fn,"wb");
  fwrite(&pipeHeader, sizeof(pipe_hdr_t), 1, pipe_f);

  // perform a DFT on the spectrum data and write the data out to the pipe file following the header




  // close both files
  fclose(pipe_f);
  //  fclose(spectrum_f);
}
