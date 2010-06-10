#ifndef _EVENT_H
#define _EVENT_H

#include "Input.H"
#include "Env.H"


class Event {
public:
  Event(const Input& input, int newValue): mInput(input), mNewValue(newValue) { 
    Env::msg(Env::CreationMsg,Env::Debug,Env::EventIndent) << "Creating Event at Input " << mInput.getType() << "("<<mInput.getChannel() << ":"<<mInput.getLine() << ") with value " << mNewValue << std::endl; 
  }
  ~Event() {}

  const Input& getInput() const {return mInput;}
  int getValue() const {return mNewValue;}

protected:
  Input mInput;
  int mNewValue;

};


#endif
