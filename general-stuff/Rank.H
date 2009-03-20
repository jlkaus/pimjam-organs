#ifndef _RANK_H
#define _RANK_H

#include <map>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#include "rankfile.H"
#include "Pipe.H"
#include "PipeSet.H"

class Rank {
public:
  Rank(std::string rankFile);
  ~Rank();

  inline PipeSet* getPipeSet(int mNote, int mOffset) {return mSets[mNote];}


protected:
  std::string mName;
  float mPitchLength;
  int mBaseWrittenNote;
  std::map<int,PipeSet*> mSets;


};



#endif
