#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Pipe.H"
#include "Env.H"
#include "rankfile.H"

Pipe::Pipe(FILE* file, long int offset) {
  Env::logMsg(Env::CreationMsg, Env::Debug, "Creating pipe from already open file");

  Env::logMsg(Env::OperationMsg, Env::Info, "Loading pipe:");
  readPipeData(file,offset);
}

Pipe::Pipe(std::string pipeFile) {
  Env::logMsg(Env::CreationMsg, Env::Debug, "Creating pipe from a file %s", pipeFile.c_str());

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

  if(fread(&pipeHeader, sizeof(pipe_hdr_t),1,fh) != 1) {
    Env::errorMsg("Not enough data for pipe header");
    throw 7;
  }

  mFundamental = pipeHeader.mFundamental;
  mSustainSamples = pipeHeader.mSustainSamples;
  mSustainDuration = pipeHeader.mSustainDuration;
  mPipeVolume = pipeHeader.mRelativeVolume;
  mDecayRate = pipeHeader.mDecayRate;

  bool noGood = false;

  if(mSustainSamples) {  
    mSustainData = new float[mSustainSamples];
    if(!mSustainData) noGood = true;
  } else {
    mSustainData = NULL;
  }

  if(noGood) {
    delete [] mSustainData;
    Env::errorMsg("Couldn't get enough memory for all the samples");
    throw 8;
  }

  if(mSustainSamples) {
    if(fread(mSustainData,sizeof(float),mSustainSamples,fh) != mSustainSamples) {
      noGood = true;
    }
  }

  if(noGood) {
    delete [] mSustainData;
    Env::errorMsg("Couldn't read all the samples for the pipe");
    throw 9;
  }
   
}
  
Pipe::~Pipe() {
  delete [] mSustainData;
  Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying pipe");
}
