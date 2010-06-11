#ifndef _EVENT_H
#define _EVENT_H

#include "Input.H"
#include "Env.H"


class Event {
public:
  Event(const Input& input, int newValue): mInput(input), mNewValue(newValue) { 
    Env::logMsg(Env::CreationMsg, Env::Debug, "Creating Event at Input %d(%d:%d) with value %d", mInput.getType(), mInput.getChannel(), mInput.getLine(), mNewValue); 
  }
  ~Event() {}

  const Input& getInput() const {return mInput;}
  int getValue() const {return mNewValue;}

protected:
  Input mInput;
  int mNewValue;

};


#endif
