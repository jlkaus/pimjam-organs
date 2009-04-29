#ifndef _INPUT_H
#define _INPUT_H

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
  Input(int channel, bool control): mType(ControlInput), mChannel(channel), mLine(-1), mRangeLow(-1), mRangeHigh(-1) { Env::msg(Env::CreationMsg,11,4) << "Creating ControlInput on channnel " << channel << std::endl; }
  Input(int channel): mType(ChannelInput), mChannel(channel), mLine(-1), mRangeLow(-1), mRangeHigh(-1) { Env::msg(Env::CreationMsg,11,4) << "Creating ChannelInput on channel " << channel << std::endl;}
  Input(int channel, int line): mType(SpecificInput), mChannel(channel), mLine(line), mRangeLow(-1), mRangeHigh(-1) {Env::msg(Env::CreationMsg,11,4) << "Creating SpecificInput on channel " << channel << ", line " << line<<std::endl;}
  Input(int channel, int rangeLow, int rangeHigh): mType(RangeInput), mChannel(channel), mLine(-1), mRangeLow(rangeLow), mRangeHigh(rangeHigh) {Env::msg(Env::CreationMsg,11,4) << "Creating RangeInput on channel " << channel << ", from "<<rangeHigh<<" to "<<rangeLow<<std::endl;}
  Input(const Input& rhs): mType(rhs.mType), mChannel(rhs.mChannel), mLine(rhs.mLine), mRangeLow(rhs.mRangeLow), mRangeHigh(rhs.mRangeHigh) {}
  ~Input() {Env::msg(Env::CreationMsg,11,4)<<"Destroying Input"<<std::endl;}

  inline const Input& operator=(const Input& rhs) { mType=rhs.mType; mChannel=rhs.mChannel; mLine=rhs.mLine; mRangeLow=rhs.mRangeLow; mRangeHigh=rhs.mRangeHigh; }
  bool matches(const Input& rhs) const;
  bool operator<(const Input& rhs) const;
  bool operator==(const Input& rhs) const;

  InputType getType() const {return mType;}
  int getChannel() const {return mChannel;}
  int getLine() const {return mLine;}

protected:
  InputType mType;
  int mChannel;
  int mLine;
  int mRangeLow;
  int mRangeHigh;
};

#endif
