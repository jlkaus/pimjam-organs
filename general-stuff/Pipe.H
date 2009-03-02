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
  Pipe(int fundamental, FILE* file, int offset);
  ~Pipe();

protected:
  // in samples:
  int mPeriodSamples;
  int mAttackSamples;
  int mReleaseSamples;

  // in cents
  int mFundamental;

  // in seconds:
  float mPeriodDuration;
  float mAttackDuration;
  float mReleaseDuration;

  // this is volume relative to the entire organ
  float mPipeVolume;

  // array of samples for attack, normalized to a peak pipe volume of 1.
  float* mAttackData;
  float* mSustainData;
  float* mReleaseData;
  void* mReserved;

};



#endif
