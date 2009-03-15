#include "Stop.H"

bool Stop::matches(const Stop& rhs) {
  return (mName == rhs.mName);
}

bool Stop::operator<(const Stop& rhs) {
  return (mName < rhs.mName);
}

bool Stop::operator==(const Stop& rhs) {
  return (mName == rhs.mName);
}
