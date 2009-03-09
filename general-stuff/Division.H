#ifndef _DIVISION_H
#define _DIVISION_H




class Division {
public:
  Division(ticpp::Element* divisionDescription);
  ~Division();



protected:

  std::map<std::string, Rank*> mRanks;
  std::map<std::string, Division*> mCoupledDivisions;

};

#endif
