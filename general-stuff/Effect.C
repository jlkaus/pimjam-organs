#include "Effect.H"
#include "Env.H"

Effect::Effect(std::string name, std::string type, float arg): mName(name), mType(NoEffect), mArg(arg) {
  Env::logMsg(Env::CreationMsg, Env::Debug, "Creating an Effect of name %s, with type %s and an argument of %f", mName.c_str(), type.c_str(), mArg);
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

Effect::Effect(std::string name, EffectType type, float arg): mName(name), mType(type), mArg(arg) {
  Env::logMsg(Env::CreationMsg, Env::Debug, "Creating an Effect of name %s, with type %d and an argument of %f", mName.c_str(), mType, mArg);
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

