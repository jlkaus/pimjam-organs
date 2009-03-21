#include <iostream>
#include <map>

#include "Rank.H"
#include "PipeSet.H"
#include "Env.H"

Rank::Rank(std::string rankFile) {
  Env::msg(Env::CreationMsg,10,6)<<"Creating rank from file "<<rankFile<<std::endl;

  Env::msg(Env::OperationMsg,1,6) << "Loading rank: "<<rankFile<<std::endl;
}

Rank::~Rank() {
  std::map<int,PipeSet*>::iterator curSet;
  for(curSet = mSets.begin(); curSet != mSets.end(); ) {
    delete (*curSet).second;
    mSets.erase(curSet++);
  }
  Env::msg(Env::CreationMsg,10,6)<<"Destroying rank from file "<<std::endl;
}
