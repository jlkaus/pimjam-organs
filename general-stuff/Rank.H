#ifndef _RANK_H
#define _RANK_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "rankfile.H"
#include "Pipe.H"
#include "PipeSet.H"
#include <map>
#include <string>

class Rank {
public:
  Rank(std::string rankFile);
  ~Rank();

  inline const PipeSet& getPipeSet(int mNote) const {return mSets[mNote];}


protected:
  wchar_t mName[256];
  float mPitchLength;
  int mBaseWrittenNote;
  std::map<int,PipeSet*> mSets;


};



#endif
