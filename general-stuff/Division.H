#ifndef _DIVISION_H
#define _DIVISION_H




class Division {
public:
  Division(ticpp::Element* divisionDescription);
  ~Division();



protected:

  std::map<std::String, Rank*> mRanks;
  std::map<std::String, Division*> mCoupledDivisions;

};

#endif
