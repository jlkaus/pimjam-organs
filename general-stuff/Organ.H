#ifndef _ORGAN_H
#define _ORGAN_H

#include <string>
#include <map>
#include "Input.H"
#include "Keyboard.H"
#include "Coupler.H"

class Division;

class Organ {
public:
  Organ(std::string organFile);
  ~Organ();

  int sendEvent(const Input& in, int newValue);
  inline Division* getDivision(std::string name) { return mDivisions[name]; }
  inline Keyboard* getKeyboard(int in) { return mKeyboards[in]; }

  int couple(Coupler& coupler);
  int decouple(Coupler& coupler);
  
protected:
  std::string mName;
  std::map<std::string, Division*> mDivisions;
  std::map<int, Keyboard*> mKeyboards;
  std::map<Input, Coupler> mCouplers;
};






#endif
