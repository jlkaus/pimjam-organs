#include "Stop.H"

bool Stop::matches(const Stop& rhs) const{
  return (mName == rhs.mName);
}

bool Stop::operator<(const Stop& rhs) const{
  return (mName < rhs.mName);
}

bool Stop::operator==(const Stop& rhs) const{
  return (mName == rhs.mName);
}
