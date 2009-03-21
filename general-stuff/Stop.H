#ifndef _STOP_H
#define _STOP_H

#include <string>
#include "Env.H"

class Stop {
public:
  Stop():mName(), mClassLength(0) {}
  Stop(std::string name, int classLength): mName(name), mClassLength(classLength) {Env::msg(Env::CreationMsg,11,4) << "Creating a Stop of name " <<mName<<" with class length "<<mClassLength<<std::endl;}
  ~Stop() {if(mName.length()) {Env::msg(Env::CreationMsg,11,4)<<"Destroying stop named "<<mName<<std::endl;}}

  inline const Stop& operator=(const Stop& rhs) { mName = rhs.mName; mClassLength = rhs.mClassLength; }
  bool matches(const Stop& rhs) const;
  bool operator<(const Stop& rhs) const;
  bool operator==(const Stop& rhs) const;


protected:
  std::string mName;
  int mClassLength;
};


#endif
