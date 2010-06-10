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

    Env::msg(Env::CreationMsg,Env::Debug,Env::OrganIndent) << "Creating organ named " << mName << std::endl;

    Env::msg(Env::OperationMsg,Env::Info,Env::OrganIndent) << "Loading organ: " <<mName<<std::endl;

    ticpp::Iterator<ticpp::Element> child("division");
    for(child = child.begin(organElement); child!=child.end(); ++child) {
      std::string divName = child->GetAttribute("name");

      mDivisions[divName] = new Division(&(*child));
    }
  } catch(ticpp::Exception& ex) {
    Env::err() << "Got a ticpp exception in Organ ctor " << ex.what() <<std::endl;
  }
}

Organ::~Organ() {
  std::map<std::string, Division*>::iterator curDiv;
  for(curDiv = mDivisions.begin(); curDiv != mDivisions.end(); ) {
    delete (*curDiv).second;
    mDivisions.erase(curDiv++);
  }
  Env::msg(Env::CreationMsg,Env::Debug,Env::OrganIndent) << "Destroying organ "<<mName<<std::endl;
}

int Organ::sendInput(const Input& in, PlayControlBlock& pcb, int newValue) {
  // go through all the Divisions until one of them handles the Input
}

