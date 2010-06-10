#include <iostream>
#include <map>

#include "Rank.H"
#include "Env.H"

Rank::Rank(std::string rankFile) {
  Env::msg(Env::CreationMsg,Env::Debug,Env::RankIndent)<<"Creating rank from file "<<rankFile<<std::endl;

  Env::msg(Env::OperationMsg,Env::Info,Env::RankIndent) << "Loading rank: "<<rankFile<<std::endl;
}

Rank::~Rank() {
  std::map<int,Pipe*>::iterator curPipe;
  for(curPipe = mPipes.begin(); curPipe != mPipes.end(); ) {
    delete (*curPipe).second;
    mPipes.erase(curPipe++);
  }
  Env::msg(Env::CreationMsg,Env::Debug,Env::RankIndent)<<"Destroying rank from file "<<std::endl;
}
