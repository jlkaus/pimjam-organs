#include "Effect.H"

bool Effect::matches(const Effect& rhs) {
  return (mName == rhs.mName);
}

bool Effect::operator<(const Effect& rhs) {
  return (mName < rhs.mName);
}

bool Effect::operator==(const Effect& rhs) {
  return (mName == rhs.mName);
}

