#include <iostream>
#include <string>

#include "Organ.H"
#include "ticpp.h"
#include "Input.H"
#include "AlsaPlayControlBlock.H"


Organ::Organ(std::string organFile) {
  try {
    ticpp::Document doc(organFile);
    doc.LoadFile();

    ticpp::Element* organElement = doc.FirstChildElement();

    std::string organName = organElement->GetAttribute("name");

    std::cout << "Reading Organ from "<<organFile<<" by name of "<< organName <<std::endl;
    ticpp::Iterator<ticpp::Element> child("division");
    for(child = child.begin(organElement); child!=child.end(); ++child) {
      std::string divName = child->GetAttribute("name");
      std::cout << "\tGoing to create a new division with a name of "<<divName<<std::endl;
      mDivisions[divName] = new Division(&(*child));
    }
  } catch(ticpp::Exception& ex) {
    std::cout << "*** Something bad happened.  Here is the exception: "<<ex.what() << std::endl;
  }
}

Organ::~Organ() {
  std::map<std::string, Division*>::iterator curDiv;
  for(curDiv = mDivisions.begin(); curDiv != mDivisions.end(); ) {
    delete (*curDiv).second;
    mDivisions.erase(curDiv++);
  }
}

int Organ::sendInput(const Input& in, AlsaPlayControlBlock& apcb) {
  // go through all the Divisions until one of them handles the Input
}

