#ifndef _COUPLER_H
#define _COUPLER_H

#include <string>
#include "Env.H"

class Coupler {
public:
  Coupler():mName(),mTarget(), mOffsetLength(0) {}
  Coupler(std::string name, std::string target, int offset):mName(name), mTarget(target), mOffsetLength(offset) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating a Coupler of name %s targeting %s with offset %d", mName.c_str(), mTarget.c_str(), mOffsetLength);}
  ~Coupler() {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying coupler named %s", mName.c_str());}

  inline std::string getTarget() const {return mTarget;}
  inline int getOffset() const {return mOffsetLength;}
  inline const Coupler& operator=(const Coupler& rhs) {mName = rhs.mName; mTarget = rhs.mTarget; mOffsetLength = rhs.mOffsetLength; }
  bool matches(const Coupler& rhs) const;
  bool operator<(const Coupler& rhs) const;
  bool operator==(const Coupler& rhs) const;

protected:
  std::string mName;
  std::string mTarget;
  int mOffsetLength;
};


#endif
