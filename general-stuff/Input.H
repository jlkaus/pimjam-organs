#ifndef _INPUT_H
#define _INPUT_H

class Input {
public:
  Input(int channel): mType(ChannelInput), mChannel(channel), mLine(-1), mRangeLow(-1), mRangeHigh(-1) {}
  Input(int channel, int line): mType(SpecificInput), mChannel(channel), mLine(line), mRangeLow(-1), mRangeHigh(-1) {}
  Input(int channel, int rangeLow, int rangeHigh): mType(RangeInput), mChannel(channel), mLine(-1), mRangeLow(rangeLow), mRangeHigh(rangeHigh) {}
  Input(const Input& rhs): mType(rhs.mType), mChannel(rhs.mChannel), mLine(rhs.mLine), mRangeLow(rhs.mRangeLow), mRangeHigh(rhs.mRangeHigh) {}
  ~Input() {}

  inline const Input& operator=(const Input& rhs) { mType=rhs.mType; mChannel=rhs.mChannel; mLine=rhs.mLine; mRangeLow=rhs.mRangeLow; mRangeHigh=rhs.mRangeHigh; }
  bool matches(const Input& rhs);
  bool operator<(const Input& rhs);
  bool operator==(const Input& rhs);


public:
  enum InputType {
    ChannelInput,
    SpecificInput,
    RangeInput
  };

protected:
  InputType mType;
  int mChannel;
  int mLine;
  int mRangeLow;
  int mRangeHigh;
};

#endif
