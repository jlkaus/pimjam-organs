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
  Input(int channel, int control_number, bool control): mType(ControlInput), mChannel(channel), mLine(control_number), mRangeLow(-1), mRangeHigh(-1) { Env::logMsg(Env::CreationMsg, Env::Debug, "Creating ControlInput on channnel %d", channel); }
  Input(int channel): mType(ChannelInput), mChannel(channel), mLine(-1), mRangeLow(-1), mRangeHigh(-1) { Env::logMsg(Env::CreationMsg, Env::Debug, "Creating ChannelInput on channel %d", channel);}
  Input(int channel, int line): mType(SpecificInput), mChannel(channel), mLine(line), mRangeLow(-1), mRangeHigh(-1) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating Specific Input on channel %d, line %d", channel, line);}
  Input(int channel, int rangeLow, int rangeHigh): mType(RangeInput), mChannel(channel), mLine(-1), mRangeLow(rangeLow), mRangeHigh(rangeHigh) {Env::logMsg(Env::CreationMsg, Env::Debug, "Creating RangeInput on channel %d, from %d to %d", channel, rangeHigh, rangeLow);}
  Input(const Input& rhs): mType(rhs.mType), mChannel(rhs.mChannel), mLine(rhs.mLine), mRangeLow(rhs.mRangeLow), mRangeHigh(rhs.mRangeHigh) {}
  ~Input() {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying Input");}

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
