#ifndef _PIPE_H
#define _PIPE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "rankfile.H"
#include <string>


// represents a real pipe of a real rank of a real division, loaded into program memory
class Pipe {
public:

  // used to create a Pipe and load in data from an already open rank file, at a given offset into that file, for a given fundamental.  The number of samples to take for each section is specified in the pipe header at the given offset.
  Pipe(FILE* file, long int offset);
  // used to create a Pipe and load in data from a pipe file, not yet open
  Pipe(std::string pipeFile);
  
  ~Pipe();

  int getNumSamples() { return mSustainSamples; }
  float* getSamples() {return mSustainData; }

protected:
  void readPipeData(FILE* fh, long int offset);

protected:
  // in hertz
  float mFundamental;
  // in seconds:
  float mSustainDuration;
  float mRsvd1;
  float mRsvd2;

  // this is volume relative to the entire organ
  float mPipeVolume;
  // volume decrease (relative to mPipeVolume) per sustain repeat (zero for organs)
  float mDecayRate;
  float mRsvd3;
  float mRsvd4;
  
  // in samples:
  int mSustainSamples;  // might be zero for no sustain (bells, chimes, etc)
  int mRsvd5;
  int mRsvd6;
  int mRsvd7;

  // array of samples for attack, normalized to a peak pipe volume of 1.
  float* mSustainData;
  void* mRsvd8;
  void* mRsvd9;
  void* mRsvd10;

};



#endif
