#ifndef _DIVISION_H
#define _DIVISION_H

#include <string>
#include <map>
#include <set>

#include "ticpp.h"
#include "Stop.H"
#include "Input.H"
#include "Keyboard.H"
#include "Coupler.H"
#include "Effect.H"
#include "Rank.H"
#include "PlayControlBlock.H"

class Division {
public:
  Division(ticpp::Element* divisionDescription);
  ~Division();

  int sendEvent(const Input& in, PlayControlBlock& pcb, int newValue);

protected:
  std::string mName;
  std::map<Input, Keyboard> mKeyboards;
  std::map<Input, Coupler> mCouplers;
  std::map<Input, Effect> mEffects;
  std::map<Input, Stop> mStops;

  std::map<Stop, Rank*> mRanks;

  std::set<Coupler> mCoupled;
  std::set<Effect> mEffected;
  std::set<Stop> mUnstopped;
};

#endif
