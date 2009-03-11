#ifndef _DIVISION_H
#define _DIVISION_H

class Division {
public:
  Division(ticpp::Element* divisionDescription);
  ~Division();

protected:
  std::string mName;
  std::map<Input, Keyboard> mKeyboards;
  std::map<Input, Coupler> mCouplers;
  std::map<Input, Effect> mEffects;
  std::map<Input, Stop> mStops;

  std::map<Stop, Rank*> mRanks;

  std::set<Coupler> mCoupled;
  std::set<Effect> mEffected;
  std::set<Stop> mUnstopped;
};

#endif
