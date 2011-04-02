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
			
      Keyboard* thisKeyboard = new Keyboard(childName, childOffset, childChannel, this);
      mKeyboards[childChannel] = thisKeyboard;

      ticpp::Element* keyboardElement = &(*childK);
      ticpp::Iterator<ticpp::Element> childCoupler("coupler");
      for(childCoupler = childCoupler.begin(keyboardElement); childCoupler != childCoupler.end(); ++childCoupler) {
        std::string couplerName = childCoupler->GetAttribute("name");
	int couplerChannel = -1;
	childCoupler->GetAttributeOrDefault("channel", &couplerChannel, -1);
	int couplerInput = -1;
	childCoupler->GetAttributeOrDefault("input", &couplerInput, -1);
	int couplerDefault = 0;
	childCoupler->GetAttributeOrDefault("default", &couplerDefault, 0);
	std::string couplerTarget = childCoupler->GetAttribute("target");
	int couplerLength = -1;
	childCoupler->GetAttributeOrDefault("length", &couplerLength, -1);

	Coupler thisCoupler(couplerName, couplerLength, thisKeyboard, getDivision(couplerTarget));
	
	// If the coupler is on by default, couple the keyboard and division
	if(couplerDefault == 1) {
		couple(thisCoupler);
	}

	// Only couplers with channels can be turned on and off
	if(couplerChannel != -1) {
		mCouplers[Input(couplerChannel, couplerInput)] = thisCoupler;
	}
      }
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
  std::map<int, Keyboard*>::iterator curKey;
  for(curKey = mKeyboards.begin(); curKey != mKeyboards.end(); ) {
    delete (*curKey).second;
    mKeyboards.erase(curKey++);
  }
  Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying organ %s", mName.c_str());
}

int Organ::sendEvent(const Input& in, int newValue) {
	
	int handled = 0;

	if(in.getType() == Input::SpecificInput) {
		
		if(handled == 0) {
			if(mKeyboards.find(in.getChannel()) != mKeyboards.end()) {
				handled = mKeyboards[in.getChannel()]->sendEvent(in, newValue);
			}
		}

		if(handled == 0) {
			if(mCouplers.find(in) != mCouplers.end()) {
				if(newValue == 1) {
					handled = couple(mCouplers[in]);
				} else if(newValue == 0) {
					handled = decouple(mCouplers[in]);
				}
			}
		}

		if(handled == 0) {
			for(std::map<std::string, Division*>::iterator iter = mDivisions.begin(); iter != mDivisions.end(); iter++) {
				if(iter->second->sendEvent(in, newValue)) {
					handled = 1;
					break;
				}
			}
		}
	}
		
	if(handled == 0) {
		Env::logMsg(Env::OperationMsg, Env::Info, "Event on input: %s with new value 0x%X not handled by organ", in.toString().c_str(), newValue);
	}

	return handled;
}

int Organ::couple(Coupler& coupler) {
	Division* division = coupler.getTarget();
	Keyboard* keyboard = coupler.getKeyboard();
	Env::logMsg(Env::OperationMsg, Env::Info, "Coupling keyboard %s to division %s at length %d", keyboard->getName().c_str(), division->getName().c_str(), coupler.getOffset());
	keyboard->coupleToDivision(division);
	division->coupleToKeyboard(keyboard, coupler.getOffset());
	return 1;
}

int Organ::decouple(Coupler& coupler) {
	Division* division = coupler.getTarget();
	Keyboard* keyboard = coupler.getKeyboard();
	Env::logMsg(Env::OperationMsg, Env::Info, "Decoupling keyboard %s from division %s", keyboard->getName().c_str(), division->getName().c_str(), coupler.getOffset());
	keyboard->decoupleFromDivision(division);
	division->decoupleFromKeyboard(keyboard);
	return 1;
}
