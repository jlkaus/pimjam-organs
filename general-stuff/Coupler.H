#ifndef _COUPLER_H
#define _COUPLER_H

class Coupler {
public:
  Coupler(std::string name, Division* target, int offset):mName(name), mTarget(target), mOffsetLength(offsset) {}
  ~Coupler() {}

protected:
  std::string mName;
  Division* mTarget;
  int mOffsetLength;
};


#endif
