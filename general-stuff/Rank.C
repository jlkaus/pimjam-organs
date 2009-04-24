#include <iostream>
#include <map>

#include "Rank.H"
#include "Env.H"

Rank::Rank(std::string rankFile) {
  Env::msg(Env::CreationMsg,10,6)<<"Creating rank from file "<<rankFile<<std::endl;

  Env::msg(Env::OperationMsg,1,6) << "Loading rank: "<<rankFile<<std::endl;
}

Rank::~Rank() {
  std::map<int,Pipe*>::iterator curPipe;
  for(curPipe = mPipes.begin(); curPipe != mPipes.end(); ) {
    delete (*curPipe).second;
    mPipes.erase(curPipe++);
  }
  Env::msg(Env::CreationMsg,10,6)<<"Destroying rank from file "<<std::endl;
}
