#ifndef _ORGAN_H
#define _ORGAN_H


class Organ {
public:
  Organ(std::String organFile);
  ~Organ();

protected:
  std::map<std::String, Division*> mDivisions;


};






#endif
