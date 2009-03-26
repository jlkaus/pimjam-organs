#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Pipe.H"
#include "Env.H"
#include "rankfile.H"

Pipe::Pipe(FILE* file, long int offset) {
  Env::msg(Env::CreationMsg,10,10)<<"Creating pipe from already open file" << std::endl;

  Env::msg(Env::OperationMsg,1,10)<<"Loading pipe:"<<std::endl;
  readPipeData(file,offset);
}

Pipe::Pipe(std::string pipeFile) {
  Env::msg(Env::CreationMsg,10,10)<<"Creating pipe from a file " <<pipeFile<< std::endl;

  FILE* fh = fopen(pipeFile.c_str(), "r");
  if(!fh) {
    throw 6;
  }

  try {
    readPipeData(fh, 0);
    fclose(fh); 
  } catch(int e) {
    fclose(fh);
    throw e;
  }
}

void Pipe::readPipeData(FILE* fh, long int offs) {
  pipe_hdr_t pipeHeader;
  fseek(fh, offs, SEEK_SET);
  if(fread(&pipeHeader, sizeof(pipeHeader),1,fh) != sizeof(pipeHeader)) {
    Env::err() << "Not enough data for pipe header" << std::endl;
    throw 7;
  }

  mFundamental = pipeHeader.mFundamental;
  mSustainSamples = pipeHeader.mSustainSamples;
  mAttackSamples = pipeHeader.mAttackSamples;
  mReleaseSamples = pipeHeader.mReleaseSamples;
  mSustainDuration = pipeHeader.mSustainDuration;
  mAttackDuration = pipeHeader.mAttackDuration;
  mReleaseDuration = pipeHeader.mReleaseDuration;
  mPipeVolume = pipeHeader.mRelativeVolume;
  mDecayRate = pipeHeader.mDecayRate;

  bool noGood = false;

  if(mSustainSamples) {  
    mSustainData = new float[mSustainSamples];
    if(!mSustainData) noGood = true;
  } else {
    mSustainData = NULL;
  }
  if(mAttackSamples) {
    mAttackData = new float[mAttackSamples];
    if(!mAttackData) noGood = true;
  } else {
    mAttackData = NULL;
  }
  if(mReleaseSamples) {
    mReleaseData = new float[mReleaseSamples];
    if(!mReleaseData) noGood = true;
  } else {
    mReleaseData = NULL;
  }

  if(noGood) {
    delete [] mSustainData;
    delete [] mAttackData;
    delete [] mReleaseData;
    Env::err() << "Couldn't get enough memory for all the samples" <<std::endl;
    throw 8;
  }

  if(mSustainSamples) {
    if(fread(mSustainData,sizeof(float),mSustainSamples,fh) != sizeof(float)*mSustainSamples) {
      noGood = true;
    }
  }
  if(mAttackSamples) {
    if(fread(mAttackData,sizeof(float),mAttackSamples,fh) != sizeof(float)*mAttackSamples) {
      noGood = true;
    }
  }
  if(mReleaseSamples) {
    if(fread(mReleaseData, sizeof(float),mReleaseSamples,fh) != sizeof(float)*mReleaseSamples) {
      noGood = true;
    }
  }

  if(noGood) {
    delete [] mSustainData;
    delete [] mAttackData;
    delete [] mReleaseData;
    Env::err() << "Couldn't read all the samples for the pipe" << std::endl;
    throw 9;
  }
   
}
  
Pipe::~Pipe() {
  delete [] mSustainData;
  delete [] mAttackData;
  delete [] mReleaseData;
  Env::msg(Env::CreationMsg,10,10)<<"Destroying pipe"<<std::endl;
}
