#include "Coupler.H"

bool Coupler::matches(const Coupler& rhs) {
  return (mName == rhs.mName);
}

bool Coupler::operator<(const Coupler& rhs) {
  return (mName < rhs.mName);
}

bool Coupler::operator==(const Coupler& rhs) {
  return (mName == rhs.mName);
}
