#ifndef _STOP_H
#define _STOP_H

#include <string>

class Stop {
public:
  Stop(std::string name, int classLength): mName(name), mClassLength(classLength) {}
  ~Stop() {}

protected:
  std::string mName;
  int mClassLength;
};


#endif
