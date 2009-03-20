#ifndef _EFFECT_H
#define _EFFECT_H

#include <string>

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
  Effect(std::string name, EffectType type, float arg): mName(name), mType(type), mArg(arg) {}
  ~Effect() {}

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
