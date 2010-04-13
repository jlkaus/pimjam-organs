#ifndef _EVENT_H
#define _EVENT_H

class Event {
public:
  Event(const Input& input, int newValue): mInput(input), mNewValue(newValue) { 
    Env::msg(Env::CreationMsg,11,4) << "Creating Event at Input " << mInput.getType() << "("<<mInput.getChannel() << ":"<<mInput.getLine() << ") with value " << mNewValue << std::endl; 
  }
  ~Event() {}

  const Input& getInput() const {return mInput;}
  int getValue() const {return mNewValue;}

protected:
  Input mInput;
  int mNewValue;

};


#endif
