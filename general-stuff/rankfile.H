#ifndef _RANKFILE_H
#define _RANKFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

// Rank files begin with a header, and then all the pipe data, as specified in pipe files, that are all brought together to make a rankfile.
// In general, a rank file would be named as <rank name>_<pitch length>.rank
// The pipefiles though might be named <source_fis_name>_<fundamental>.pipe

struct pipe_hdr_t {
  float mFundamental;    // actual frequency of the fundamental
  float mSustainDuration;
  float mRsvd1;
  float mRsvd2;

  float mRelativeVolume;
  float mDecayRate;   // volume decrease per sustain repeat. Probably 0 for organ pipes.
  float mRsvd3;
  float mRsvd4;

  int mSustainSamples;
  int mRsvd5;
  int mRsvd6;
  int mRsvd7;
  

  // Then: the actual array of floats for the sustain
  // float mSustainData[];
};

struct pipe_desc_t {
  int mWrittenNote;  // offset from C2 of the key that when pressed would play this note for the given rank
  int mRsvd1;
  long int mOffset;  // offset into this file to seek to in order to find the start of this pipe's data, at the pipe_hdr_t record.
};

struct rank_hdr_t {
  wchar_t mRankName[256];   // zero padded unicode string that is the name of this rank of pipes.  Would not include the pitch length number.  Needn't be unique amongst all ranks.  This, combined with the mPitchLength field is the true name of this rank, and what should set it apart from other ranks.
  float mPitchLength;    // pitch class length for this rank
  float mRsvd1;
  float mRsvd2;
  float mRsvd3;
  
  int mNumPipes; // number of pipe sets in this rank
  int mLowestNormalNote; // offset of pipe that would play when the C2 key was pressed
  int mRsvd4;
  int mRsvd5;

  // Then: the array of pipe_desc_t records for each pipe.
  // pipe_desc_t mPipes[];
};

struct dft_freq_point_t {
  float mFrequency;    // frequency in hertz
  float mRealFactor;   // real part of the complex coefficient for the given frequency
  float mImagFactor;   // imaginary part of the complex coefficient for the given frequency
};

#endif
