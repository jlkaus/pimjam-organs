#ifndef _ORGAN_H
#define _ORGAN_H

#include <string>
#include <map>
#include "Division.H"
#include "Input.H"
#include "PlayControlBlock.H"


class Organ {
public:
  Organ(std::string organFile);
  ~Organ();

  int sendEvent(const Input& in, int newValue);
 inline	Division* getDivision(std::string name) { return mDivisions[name]; } 
  
protected:
  std::string mName;
  PlayControlBlock mPlayControlBlock;
  std::map<std::string, Division*> mDivisions;


};






#endif
