#ifndef _DIVISION_H
#define _DIVISION_H

#include <string>
#include <map>
#include <set>

#include "ticpp.h"
#include "Stop.H"
#include "Input.H"
#include "Event.H"
#include "Effect.H"
#include "Rank.H"
#include "PlayControlBlock.H"

class Keyboard;

class Division {
public:
  Division(ticpp::Element* divisionDescription);
  ~Division();

  enum MidiControlTypes{
	MidiControlTypes_ModulationWheel = 0x01,
	MidiControlTypes_ExpressionPedal = 0x0B
  };
	
  inline std::string getName() const {return mName;}
  inline float getExpressionValue() const {return mExpressionValue;}
  inline void coupleToKeyboard(Keyboard* keyboard, int offset) {mCoupled[keyboard] = offset;}
  inline void decoupleFromKeyboard(Keyboard* keyboard) {mCoupled.erase(keyboard);}
  int keyboardStateChange(Keyboard* keyboard);
  Event::EventStatus sendEvent(const Input& in, int newValue);

protected:
  std::string mName;
  int mControlChannel;
  int mExpressionMax;
  std::map<Input, Effect> mEffects;
  std::map<Input, Stop> mStops;

  std::map<Stop, Rank*> mRanks;

  std::set<Effect> mEffected;
  std::set<Stop> mUnstopped;
  std::map<Keyboard*, int> mCoupled;
  float mExpressionValue;
};

#endif
