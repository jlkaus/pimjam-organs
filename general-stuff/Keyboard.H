#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <string>
#include "Env.H"

class Organ;

class Keyboard {
public:
  Keyboard():mName(),mOffsetLength(0),mOrgan(NULL) {}
  Keyboard(std::string name, int offset, Organ* organ): mName(name), mOffsetLength(offset), mOrgan(organ) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating a Keyboard of name %s with offset %d", mName.c_str(), mOffsetLength);}
  ~Keyboard() {if(mName.length()) {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying keyboard named %s", mName.c_str());}}

protected:
  std::string mName;
  int mOffsetLength;
  Organ* mOrgan;
};

#endif
