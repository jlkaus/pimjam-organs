#ifndef _INPUT_H
#define _INPUT_H

#include <assert.h>
#include "Env.H"

class Input {
public:
  enum InputType {
    ControlInput,
    ChannelInput,
    SpecificInput,
    RangeInput
  };

public:
  Input(int channel, int control_number, bool control): mType(ControlInput), mChannel(channel), mLine(control_number), mRangeLow(-1), mRangeHigh(-1) { Env::msg(Env::CreationMsg,Env::Debug,Env::InputIndent) << "Creating ControlInput on channnel " << channel << std::endl; }
  Input(int channel): mType(ChannelInput), mChannel(channel), mLine(-1), mRangeLow(-1), mRangeHigh(-1) { Env::msg(Env::CreationMsg,Env::Debug,Env::InputIndent) << "Creating ChannelInput on channel " << channel << std::endl;}
  Input(int channel, int line): mType(SpecificInput), mChannel(channel), mLine(line), mRangeLow(-1), mRangeHigh(-1) {Env::msg(Env::CreationMsg,Env::Debug,Env::InputIndent) << "Creating SpecificInput on channel " << channel << ", line " << line<<std::endl;}
  Input(int channel, int rangeLow, int rangeHigh): mType(RangeInput), mChannel(channel), mLine(-1), mRangeLow(rangeLow), mRangeHigh(rangeHigh) {Env::msg(Env::CreationMsg,Env::Debug,Env::InputIndent) << "Creating RangeInput on channel " << channel << ", from "<<rangeHigh<<" to "<<rangeLow<<std::endl;}
  Input(const Input& rhs): mType(rhs.mType), mChannel(rhs.mChannel), mLine(rhs.mLine), mRangeLow(rhs.mRangeLow), mRangeHigh(rhs.mRangeHigh) {}
  ~Input() {Env::msg(Env::CreationMsg,Env::Debug,Env::InputIndent)<<"Destroying Input"<<std::endl;}

  inline const Input& operator=(const Input& rhs) { mType=rhs.mType; mChannel=rhs.mChannel; mLine=rhs.mLine; mRangeLow=rhs.mRangeLow; mRangeHigh=rhs.mRangeHigh; }
  bool matches(const Input& rhs) const;
  bool operator<(const Input& rhs) const;
  bool operator==(const Input& rhs) const;

  InputType getType() const {return mType;}
  int getChannel() const {return mChannel;}
  int getLine() const {assert(mType==SpecificInput); return mLine;}
  int getControlNumber() const {assert(mType==ControlInput); return mLine;}

protected:
  InputType mType;
  int mChannel;
  int mLine;
  int mRangeLow;
  int mRangeHigh;
};

#endif
