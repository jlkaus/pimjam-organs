#ifndef _RANKFILE_H
#define _RANKFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

// Rank files begin with a header, and then all the pipe data, as specified in pipe files, that are all brought together to make a rankfile.
// In general, a rank file would be named as <rank name>_<pitch length>.rank
// The pipefiles though might be named <rank name>_<fundamental>.pipe
// This emphasizes that a given pipe might be used in a variety of actual ranks, but usually they would all be of the same rank name, just a different pitch length class.  (Not necessarily true: mixed ranks might contain a pipe that is also in an unmixed rank).

struct pipe_hdr_t {
  int mFundamental;    // actual frequency of the fundamental, in cents
  int mSustainSamples;  // for a bell, for instance, this might be zero
  int mAttackSamples;
  int mReleaseSamples;

  float mSustainDuration;
  float mAttackDuration;
  float mReleaseDuration;
  float mRsvd1;
  
  float mRelativeVolume;
  float mDecayRate;   // volume decrease per sustain repeat
  float mRsvd2;
  float mRsvd3;

  // Then: the actual array of floats for the sustain, then for the attack, then for the release
  // float mSustainData[];
  // float mAttackData[];
  // float mReleaseData[];
};

struct pipe_desc_t {
  int mWrittenNote;  // freqency of this pipe as it would be written on the page, in cents.  For pipes of a given set, this would all be the same, even if the pipes in that set are not all of the same actual frequency
  int mRsvd1;
  long int mOffset;  // offset into this file to seek to in order to find the start of this pipe's data, at the pipe_hdr_t record.
};

struct rank_hdr_t {
  wchar_t mRankName[256];   // zero padded unicode string that is the name of this rank of pipes.  Would not include the pitch length number.  Needn't be unique amongst all ranks.  This, combined with the mPitchLength field is the true name of this rank, and what should set it apart from other ranks.
  float mPitchLength;    // pitch class length for this rank
  float mRsvd1;
  float mRsvd2;
  float mRsvd3;
  
  int mBaseFrequency; // lowest pipe note for this rank, in cents.  
  int mNumPipeSets; // number of pipe sets in this rank
  int mPipesPerSet;  // number of actual pipes in each pipe set for this rank (usually 1... can be used for combined ranks of multiple pipes per note)
  int mRsvd4;

  // Then: the array of pipe_desc_t records for each pipe.  All pipes in a set are listed consecutively.
  // pipe_desc_t mPipes[];
};




#endif
