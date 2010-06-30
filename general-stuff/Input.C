#include "Input.H"
#include "Env.H"

#include <string>

bool Input::matches(const Input& rhs) const{
  // channel has to match regardless of what type we are
  if(mChannel == rhs.mChannel) {
    if(mType == ControlInput && rhs.mType == ControlInput) {
      return true;
    } else if(mType == ChannelInput && rhs.mType != ControlInput) {
      return true;
    } else if(mType == RangeInput && rhs.mType != ControlInput) {
      // if we are a range type, then the rhs must be a specific within our range, or a range within our range
      if(rhs.mLine >= mRangeLow && rhs.mLine <= mRangeHigh) {
	return true;
      }
      if(rhs.mRangeLow >= mRangeLow && rhs.mRangeHigh <= mRangeHigh) {
	return true;
      }
    } else {
      // if we are specific type, then the rhs must be a specific within our range as well
      if(mLine == rhs.mLine) {
	return true;
      }
    }
  }
  return false;
}

bool Input::operator<(const Input& rhs) const{
  if(mType == SpecificInput) {
    if(rhs.mType == SpecificInput) {
      if(mChannel == rhs.mChannel) {
	return (mLine < rhs.mLine);
      } else {
	return (mChannel < rhs.mChannel);
      }
    } else {
      return true;
    }
  } else if(mType == RangeInput) {
    if(rhs.mType == RangeInput) {
      if(mChannel == rhs.mChannel) {
	return (mRangeLow < rhs.mRangeLow);
      } else {
	return (mChannel < rhs.mChannel);
      }
    } else if(rhs.mType == SpecificInput) {
      return false;
    } else {
      return true;
    }
  } else if(mType == ChannelInput) {
    if(rhs.mType == ChannelInput) {
      return (mChannel < rhs.mChannel);
    } else if(rhs.mType == ControlInput) {
      return true;
    } else {
      return false;
    }
  } else {
    if(rhs.mType == ControlInput) {
      return (mChannel < rhs.mChannel);
    } else {
      return false;
    }
  }
}

bool Input::operator==(const Input& rhs) const{
  return matches(rhs);
}

std::string Input::toString() const 
{
	char stringbuffer[100];
	int string_size = 0;
	switch(mType) {
		case ControlInput:
		case SpecificInput:
			string_size = snprintf(stringbuffer, sizeof(stringbuffer), "%d(%d:%d)", mType, mChannel, mLine);
			break;
		case ChannelInput:
			string_size = snprintf(stringbuffer, sizeof(stringbuffer), "%d(%d)", mType, mChannel);
			break;
		case RangeInput:
			string_size = snprintf(stringbuffer, sizeof(stringbuffer), "%d(%d:%d-%d)", mType, mChannel, mRangeLow, mRangeHigh);
			break;
	}
	std::string retString(stringbuffer, string_size);
	return retString;
}
