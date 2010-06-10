#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <string>
#include "Env.H"

class Keyboard {
public:
  Keyboard():mName(),mOffsetLength(0) {}
  Keyboard(std::string name, int offset): mName(name), mOffsetLength(offset) {Env::msg(Env::CreationMsg,Env::Debug,Env::KeyboardIndent) << "Creating a Keyboard of name " << mName << " with offset "<<mOffsetLength << std::endl;}
  ~Keyboard() {if(mName.length()) {Env::msg(Env::CreationMsg,Env::Debug,Env::KeyboardIndent)<<"Destroying keyboard named "<<mName<<std::endl;}}

protected:
  std::string mName;
  int mOffsetLength;
};

#endif
