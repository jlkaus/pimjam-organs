#include <iostream>
#include <map>

#include "Rank.H"
#include "PipeSet.H"


Rank::Rank(std::string rankFile) {
  std::cout << "\t\t\tGoing to read in the file from " << rankFile << std::endl;

}

Rank::~Rank() {
  std::map<int,PipeSet*>::iterator curSet;
  for(curSet = mSets.begin(); curSet != mSets.end(); ) {
    delete (*curSet).second;
    mSets.erase(curSet++);
  }
}
