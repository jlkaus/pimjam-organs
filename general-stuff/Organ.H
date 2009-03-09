#ifndef _ORGAN_H
#define _ORGAN_H


class Organ {
public:
  Organ(std::string organFile);
  ~Organ();

protected:
  std::map<std::string, Division*> mDivisions;


};






#endif
