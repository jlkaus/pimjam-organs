#ifndef _COUPLER_H
#define _COUPLER_H

#include <string>
#include "Env.H"

class Keyboard;
class Division;

class Coupler {
public:
  Coupler(): mName(), mTarget(), mOffsetLength(0), mKeyboard(NULL) {}
  Coupler(std::string name, int offset, Keyboard* keyboard, Division* target):mName(name), mOffsetLength(offset), mKeyboard(keyboard), mTarget(target) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating a Coupler of name %s with offset %d", mName.c_str(), mOffsetLength);}
  ~Coupler() {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying coupler named %s", mName.c_str());}

  inline std::string getName() const {return mName;}
  inline Keyboard* getKeyboard() const {return mKeyboard;}
  inline Division* getTarget() const {return mTarget;}
  inline int getOffset() const {return mOffsetLength;}
  inline const Coupler& operator=(const Coupler& rhs) {mName = rhs.mName; mTarget = rhs.mTarget; mOffsetLength = rhs.mOffsetLength; mKeyboard = rhs.mKeyboard;}
  bool matches(const Coupler& rhs) const;
  bool operator<(const Coupler& rhs) const;
  bool operator==(const Coupler& rhs) const;

protected:
  std::string mName;
  int mOffsetLength;
  Keyboard* mKeyboard;
  Division* mTarget;
};


#endif
