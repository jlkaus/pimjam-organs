#include <iostream>
#include <string>

#include "Organ.H"
#include "ticpp.h"
#include "Input.H"
#include "Env.H"
#include "Division.H"
#include "Keyboard.H"

Organ::Organ(std::string organFile) {
  try {
    ticpp::Document doc(organFile);
    doc.LoadFile();

    ticpp::Element* organElement = doc.FirstChildElement();

    mName = organElement->GetAttribute("name");

    Env::logMsg(Env::CreationMsg, Env::Debug, "Creating organ named %s", mName.c_str());

    Env::logMsg(Env::OperationMsg,Env::Info, "Loading organ: %s", mName.c_str());

    ticpp::Iterator<ticpp::Element> dchild("division");
    for(dchild = dchild.begin(organElement); dchild!=dchild.end(); ++dchild) {
      std::string divName = dchild->GetAttribute("name");
      mDivisions[divName] = new Division(&(*dchild));
    }

    ticpp::Iterator<ticpp::Element> childK("keyboard");
    for(childK = childK.begin(organElement); childK!= childK.end(); ++childK) {
      std::string childName = childK->GetAttribute("name");
      int childChannel = -1;
      childK->GetAttributeOrDefault("channel", &childChannel,-1);
      int childOffset = 0;
      childK->GetAttributeOrDefault("length",&childOffset,0);
			
      mKeyboards[Input(childChannel)] = Keyboard(childName, childOffset, this);
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

int Organ::sendEvent(const Input& in, int newValue) {
  // go through all the Divisions until one of them handles the Input
}

