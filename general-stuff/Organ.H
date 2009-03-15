#ifndef _ORGAN_H
#define _ORGAN_H

#include <string>
#include <map>
#include "Division.H"
#include "Input.H"
#include "AlsaPlayControlBlock.H"


class Organ {
public:
  Organ(std::string organFile);
  ~Organ();

  int sendInput(const Input& in, AlsaPlayControlBlock& apcb);

protected:
  std::map<std::string, Division*> mDivisions;


};






#endif
