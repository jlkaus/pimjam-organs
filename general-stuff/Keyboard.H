#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <string>


class Keyboard {
public:
  Keyboard(std::string name, int offset): mName(name), mOffsetLength(offset) {}
  ~Keyboard() {}

protected:
  std::string mName;
  int mOffsetLength;
};

#endif
