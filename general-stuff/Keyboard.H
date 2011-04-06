#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <string>
#include <set>
#include "Env.H"
#include "Input.H"
#include "Event.H"

class Organ;
class Division;

class Keyboard {
public:
  Keyboard():mName(),mOffsetLength(0),mOrgan(NULL) {}
  Keyboard(std::string name, int offset, int channel, Organ* organ): mName(name), mOffsetLength(offset), mChannel(channel), mOrgan(organ) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating a Keyboard of name %s with offset %d", mName.c_str(), mOffsetLength);}
  ~Keyboard() {if(mName.length()) {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying keyboard named %s", mName.c_str());}}

  inline std::string getName() const {return mName;}
  inline int getChannel() const {return mChannel;}
  inline void coupleToDivision(Division* division) {mCoupledDivisions.insert(division);}
  inline void decoupleFromDivision(Division* division) {mCoupledDivisions.erase(division);}
  Event::EventStatus sendEvent(const Input& in, int newValue);

protected:

  void notifyDivisionsOfStateChange();

  std::string mName;
  int mChannel;
  int mOffsetLength;
  Organ* mOrgan;
  std::set<Division*> mCoupledDivisions;
  std::set<int> mPressedKeys;
};

#endif
