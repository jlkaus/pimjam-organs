#ifndef _EVENT_H
#define _EVENT_H

#include "Input.H"
#include "Env.H"

#include <string>

class Event {
public:
  Event(const Input& input, int newValue): mInput(input), mNewValue(newValue) { 
    Env::logMsg(Env::CreationMsg, Env::Debug, "Creating Event at Input %s with value %d", mInput.toString().c_str(), mNewValue); 
  }
  ~Event() {}

  const Input& getInput() const {return mInput;}
  int getValue() const {return mNewValue;}

protected:
  Input mInput;
  int mNewValue;

};


#endif
