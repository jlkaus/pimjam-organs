#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <string>
#include "Env.H"

class Keyboard {
public:
  Keyboard():mName(),mOffsetLength(0) {}
  Keyboard(std::string name, int offset): mName(name), mOffsetLength(offset) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating a Keyboard of name %s with offset %d", mName.c_str(), mOffsetLength);}
  ~Keyboard() {if(mName.length()) {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying keyboard named %s", mName.c_str());}}

protected:
  std::string mName;
  int mOffsetLength;
};

#endif
