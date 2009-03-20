#ifndef _STOP_H
#define _STOP_H

#include <string>

class Stop {
public:
  Stop():mName(), mClassLength(0) {}
  Stop(std::string name, int classLength): mName(name), mClassLength(classLength) {}
  ~Stop() {}

  inline const Stop& operator=(const Stop& rhs) { mName = rhs.mName; mClassLength = rhs.mClassLength; }
  bool matches(const Stop& rhs) const;
  bool operator<(const Stop& rhs) const;
  bool operator==(const Stop& rhs) const;


protected:
  std::string mName;
  int mClassLength;
};


#endif
