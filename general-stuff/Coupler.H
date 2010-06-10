#ifndef _COUPLER_H
#define _COUPLER_H

#include <string>
#include "Env.H"

class Coupler {
public:
  Coupler():mName(),mTarget(), mOffsetLength(0) {}
  Coupler(std::string name, std::string target, int offset):mName(name), mTarget(target), mOffsetLength(offset) {Env::msg(Env::CreationMsg,Env::Debug,Env::CouplerIndent) << "Creating a Coupler of name " << mName << " targeting " << mTarget << " with offset "<<mOffsetLength << std::endl;}
  ~Coupler() {Env::msg(Env::CreationMsg,Env::Debug,Env::CouplerIndent)<<"Destroying coupler named "<<mName<<std::endl;}

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
