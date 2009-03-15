#ifndef _EFFECT_H
#define _EFFECT_H

#include <string>

class Effect {
public:
  Effect(std::string name, EffectType type, float arg): mName(name), mType(type), mArg(arg) {}
  ~Effect() {}

public:
  enum EffectType {
    SustainEffect,
    VibratoEffect
  };



protected:
  std::string mName;
  EffectType mType;
  float mArg;
};


#endif
