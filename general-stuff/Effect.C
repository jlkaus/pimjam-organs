#include "Effect.H"

Effect::Effect(std::string name, std::string type, float arg): mName(name), mType(NoEffect), mArg(arg) {
  if(type == "sustain") { 
    mType = SustainEffect; 
  } else if(type == "vibrato") {
    mType = VibratoEffect;
  } else if(type == "volume") {
    mType = VolumeEffect;
  } else {
    throw 4;
  }
}

bool Effect::matches(const Effect& rhs) const{
  return (mName == rhs.mName);
}

bool Effect::operator<(const Effect& rhs) const{
  return (mName < rhs.mName);
}

bool Effect::operator==(const Effect& rhs) const{
  return (mName == rhs.mName);
}

