#ifndef _PIPE_H
#define _PIPE_H

// represents a real pipe of a real rank of a real division, loaded into program memory
class Pipe {
public:
  Pipe(int fundamental, FILE* file, int offset, int attackLen, int sustainLen, int releaseLen);
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
