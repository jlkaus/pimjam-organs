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

class Rank {
public:
  Rank(std::string rankFile);
  ~Rank();

  inline Pipe* getPipe(int mNote, int mOffset) {return mPipes[mNote];}

protected:
  void readRankData(FILE* fh, long int offset);

protected:
  std::wstring mName;
  float mPitchLength;
  int mBaseWrittenNote;
  std::map<int,Pipe*> mPipes;


};



#endif
