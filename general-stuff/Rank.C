#include <iostream>
#include <map>

#include "Rank.H"
#include "Env.H"

Rank::Rank(std::string rankFile) {
  Env::logMsg(Env::CreationMsg, Env::Debug, "Creating rank from file %s", rankFile.c_str());

  Env::logMsg(Env::OperationMsg, Env::Info, "Loading rank: %s", rankFile.c_str());
}

Rank::~Rank() {
  std::map<int,Pipe*>::iterator curPipe;
  for(curPipe = mPipes.begin(); curPipe != mPipes.end(); ) {
    delete (*curPipe).second;
    mPipes.erase(curPipe++);
  }
  Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying rank");
}
