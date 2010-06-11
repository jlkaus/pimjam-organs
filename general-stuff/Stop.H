#ifndef _STOP_H
#define _STOP_H

#include <string>
#include "Env.H"

class Stop {
public:
  Stop():mName(), mClassLength(0) {}
  Stop(std::string name, int classLength): mName(name), mClassLength(classLength) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating a Stop of name %s with class length %d", mName.c_str(), mClassLength);}
  ~Stop() {if(mName.length()) {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying stop named %s", mName.c_str());}}

  inline const Stop& operator=(const Stop& rhs) { mName = rhs.mName; mClassLength = rhs.mClassLength; }
  bool matches(const Stop& rhs) const;
  bool operator<(const Stop& rhs) const;
  bool operator==(const Stop& rhs) const;


protected:
  std::string mName;
  int mClassLength;
};


#endif
