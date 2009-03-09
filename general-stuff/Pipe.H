#ifndef _PIPE_H
#define _PIPE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "rankfile.H"


// represents a real pipe of a real rank of a real division, loaded into program memory
class Pipe {
public:

  // used to create a Pipe and load in data from an already open rank file, at a given offset into that file, for a given fundamental.  The number of samples to take for each section is specified in the pipe header at the given offset.
  Pipe(FILE* file, int offset);
  // used to create a Pipe and load in data from a pipe file, not yet open
  Pipe(std::string pipeFile);
  
  ~Pipe();

protected:
  // in cents
  int mFundamental;
  
  // in samples:
  int mSustainSamples;  // might be zero for no sustain (bells, chimes, etc)
  int mAttackSamples;
  int mReleaseSamples;

  // in seconds:
  float mSustainDuration;
  float mAttackDuration;
  float mReleaseDuration;
  float mRsvd1;
  
  // this is volume relative to the entire organ
  float mPipeVolume;
  // volume decrease (relative to mPipeVolume) per sustain repeat (zero for organs)
  float mDecayRate;
  float mRsvd2;
  float mRsvd3;
  
  // array of samples for attack, normalized to a peak pipe volume of 1.
  float* mSustainData;
  float* mAttackData;
  float* mReleaseData;
  void* mRsvd4;

};



#endif
