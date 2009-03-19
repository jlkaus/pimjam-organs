#ifndef _COUPLER_H
#define _COUPLER_H

#include <string>
#include "Division.H"


class Coupler {
public:
  Coupler(std::string name, std::string target, int offset):mName(name), mTarget(target), mOffsetLength(offsset) {}
  ~Coupler() {}

  inline std::string getTarget() const {return mTarget;}
  inline int getOffset() const {return mOffsetLength;}
  inline const Coupler& operator=(const Coupler& rhs) {mName = rhs.mName; mTarget = rhs.mTarget; mOffsetLength = rhs.mOffsetLength; }
  bool matches(const Coupler& rhs);
  bool operator<(const Coupler& rhs);
  bool operator==(const Coupler& rhs);

protected:
  std::string mName;
  std::string mTarget;
  int mOffsetLength;
};


#endif
