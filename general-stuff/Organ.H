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

  int sendInput(const Input& in, PlayControlBlock& pcb, int newValue);
 inline	Division* getDivision(std::string name) { return mDivisions[name]; } 
  
protected:
  std::string mName;
  std::map<std::string, Division*> mDivisions;


};






#endif
