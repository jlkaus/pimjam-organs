#include <iostream>
#include <map>

#include "Rank.H"
#include "Pipe.H"
#include "rankfile.H"
#include "Env.H"
#include "Exceptions.H"

Rank::Rank(std::string rankFile) {
  Env::logMsg(Env::CreationMsg, Env::Debug, "Creating rank from file %s", rankFile.c_str());

  Env::logMsg(Env::OperationMsg, Env::Info, "Loading rank: %s", rankFile.c_str());
  
  FILE* fh = fopen(rankFile.c_str(), "r");
  if(!fh) {
    throw OrganIntException(60);
  }

  try {
    readRankData(fh, 0);
    fclose(fh); 
  } catch(int e) {
    fclose(fh);
    throw e;
  }
  
}

void Rank::readRankData(FILE* fh, long int offset)
{
  rank_hdr_t rankHeader;
  fseek(fh, offset, SEEK_SET);

  if(fread(&rankHeader, sizeof(rank_hdr_t),1,fh) != 1) {
    Env::errorMsg("Not enough data for rank header");
    throw 70;
  }

  mName = rankHeader.mRankName;
  mPitchLength = rankHeader.mPitchLength;
  mBaseWrittenNote = rankHeader.mLowestNormalNote;

  bool noGood = false;

  int pipeCount = rankHeader.mNumPipes;

  pipe_desc_t *pipeDescs = new pipe_desc_t[pipeCount];
  if(!pipeDescs) noGood = true;

  if(noGood) {
    delete [] pipeDescs;
    Env::errorMsg("Couldn't get enough memory for all the pipe descriptors in the rank");
    throw 80;
  }

  if(fread(pipeDescs, sizeof(pipe_desc_t), pipeCount, fh) != pipeCount) {
    noGood = true;
  }

  if(noGood) {
    delete [] pipeDescs;
    Env::errorMsg("Couldn't read all the pipe descriptors for the rank");
    throw 90;
  }

  try {
    for(int i = 0; i < pipeCount; ++i) {
      Pipe *tempPipe = new Pipe(fh, pipeDescs[i].mOffset);
      mPipes[pipeDescs[i].mWrittenNote] = tempPipe;
    }
  } catch(int e) {
    delete [] pipeDescs;
    throw e;
  }

  delete [] pipeDescs;
}

Rank::~Rank() {
  std::map<int,Pipe*>::iterator curPipe;
  for(curPipe = mPipes.begin(); curPipe != mPipes.end(); ) {
    delete (*curPipe).second;
    mPipes.erase(curPipe++);
  }
  Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying rank");
}
