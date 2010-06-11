#ifndef _EFFECT_H
#define _EFFECT_H 

#include <string>
#include "Env.H"

class Effect {
public:
  enum EffectType {
    NoEffect,
    SustainEffect,
    VibratoEffect,
    VolumeEffect

  };


public:
  Effect(): mName(), mType(NoEffect), mArg(0.0) {}
  Effect(std::string name, std::string type, float arg);
  Effect(std::string name, EffectType type, float arg);
  ~Effect() {if(mName.length()) {Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying effect named %s", mName.c_str());}}

  inline const Effect& operator=(const Effect& rhs) { mName = rhs.mName; mType = rhs.mType; mArg = rhs.mArg; }
  bool matches(const Effect& rhs) const;
  bool operator<(const Effect& rhs) const;
  bool operator==(const Effect& rhs) const;




protected:
  std::string mName;
  EffectType mType;
  float mArg;
};


#endif
