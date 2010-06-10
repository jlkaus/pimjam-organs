#include "Effect.H"
#include "Env.H"

Effect::Effect(std::string name, std::string type, float arg): mName(name), mType(NoEffect), mArg(arg) {
  Env::msg(Env::CreationMsg,Env::Debug,Env::EffectIndent) << "Creating an Effect of name "<<mName<< ", with type "<<type<<" and an argument of " << mArg << std::endl;
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
  Env::msg(Env::CreationMsg,Env::Debug,Env::EffectIndent) << "Creating an Effect of name "<<mName<< ", with type "<<(int)mType<<" and an argument of " << mArg << std::endl;
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

