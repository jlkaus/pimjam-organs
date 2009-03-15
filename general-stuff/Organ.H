#ifndef _ORGAN_H
#define _ORGAN_H

#include <string>
#include <map>
#include "Division.H"

class Organ {
public:
  Organ(std::string organFile);
  ~Organ();

protected:
  std::map<std::string, Division*> mDivisions;


};






#endif
