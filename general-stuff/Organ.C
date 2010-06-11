#include <iostream>
#include <string>

#include "Organ.H"
#include "ticpp.h"
#include "Input.H"
#include "PlayControlBlock.H"
#include "Env.H"


Organ::Organ(std::string organFile) {
  try {
    ticpp::Document doc(organFile);
    doc.LoadFile();

    ticpp::Element* organElement = doc.FirstChildElement();

    mName = organElement->GetAttribute("name");

    Env::logMsg(Env::CreationMsg, Env::Debug, "Creating organ named %s", mName.c_str());

    Env::logMsg(Env::OperationMsg,Env::Info, "Loading organ: %s", mName.c_str());

    ticpp::Iterator<ticpp::Element> child("division");
    for(child = child.begin(organElement); child!=child.end(); ++child) {
      std::string divName = child->GetAttribute("name");

      mDivisions[divName] = new Division(&(*child));
    }
  } catch(ticpp::Exception& ex) {
    Env::errorMsg("Got a ticpp exception in Organ ctor %d", ex.what());
  }
}

Organ::~Organ() {
  std::map<std::string, Division*>::iterator curDiv;
  for(curDiv = mDivisions.begin(); curDiv != mDivisions.end(); ) {
    delete (*curDiv).second;
    mDivisions.erase(curDiv++);
  }
  Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying organ %s", mName.c_str());
}

int Organ::sendInput(const Input& in, PlayControlBlock& pcb, int newValue) {
  // go through all the Divisions until one of them handles the Input
}

