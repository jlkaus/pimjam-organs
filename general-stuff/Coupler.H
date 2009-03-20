#ifndef _COUPLER_H
#define _COUPLER_H

#include <string>


class Coupler {
public:
  Coupler():mName(),mTarget(), mOffsetLength(0) {}
  Coupler(std::string name, std::string target, int offset):mName(name), mTarget(target), mOffsetLength(offset) {}
  ~Coupler() {}

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
