#ifndef _EFFECT_H
#define _EFFECT_H

#include <string>

class Effect {
public:
  Effect(std::string name, EffectType type, float arg): mName(name), mType(type), mArg(arg) {}
  ~Effect() {}

  inline const Effect& operator=(const Effect& rhs) { mName = rhs.mName; mType = rhs.mType; mArg = rhs.mArg; }
  bool matches(const Effect& rhs);
  bool operator<(const Effect& rhs);
  bool operator==(const Effect& rhs);

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
