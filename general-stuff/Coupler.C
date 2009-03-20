#include "Coupler.H"

bool Coupler::matches(const Coupler& rhs) const{
  return (mName == rhs.mName);
}

bool Coupler::operator<(const Coupler& rhs) const{
  return (mName < rhs.mName);
}

bool Coupler::operator==(const Coupler& rhs) const{
  return (mName == rhs.mName);
}
