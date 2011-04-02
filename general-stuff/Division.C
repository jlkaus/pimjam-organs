#include <map>
#include <iostream>
#include <set>
#include "ticpp.h"

#include "Division.H"
#include "Rank.H"
#include "Keyboard.H"
#include "Input.H"
#include "PlayControlBlock.H"
#include "Env.H"

Division::Division(ticpp::Element* divisionDescription) :
  mControlChannel(-1),
  mExpressionMax(-1),
  mExpressionValue(0.0)
{
  try {
    mName = divisionDescription->GetAttribute("name");
    divisionDescription->GetAttributeOrDefault("control_channel", &mControlChannel, -1);
    divisionDescription->GetAttributeOrDefault("expression_max", &mExpressionMax, -1);

    Env::logMsg(Env::CreationMsg, Env::Debug, "Creating division named %s", mName.c_str());

    Env::logMsg(Env::OperationMsg, Env::Info, "Loading division: %s", mName.c_str());

    ticpp::Iterator<ticpp::Element> childE("effect");
    for(childE = childE.begin(divisionDescription); childE!= childE.end(); ++childE) {
      std::string childName = childE->GetAttribute("name");
      std::string childType = childE->GetAttribute("type");
      int childChannel = -1;
      childE->GetAttributeOrDefault("channel",&childChannel, -1);
      int childInput = -1;
      childE->GetAttributeOrDefault("input",&childInput, -1);
      float childArg = 0.0;
      childE->GetAttributeOrDefault("arg",&childArg, 0.0);
			
      mEffects[Input(childChannel,childInput)] = Effect(childName, childType, childArg);
    }

    ticpp::Iterator<ticpp::Element> childS("stop");
    for(childS = childS.begin(divisionDescription); childS!= childS.end(); ++childS) {
      std::string childName = childS->GetAttribute("name");
      std::string childRank = childS->GetAttribute("rank");
      int childChannel = -1;
      childS->GetAttributeOrDefault("channel",&childChannel,-1);
      int childInput = -1;
      childS->GetAttributeOrDefault("input",&childInput,-1);
      int childLength = 0;
      childS->GetAttributeOrDefault("length",&childLength,0);
			
      mStops[Input(childChannel,childInput)] = Stop(childName, childLength);
      
      mRanks[Stop(childName, childLength)] = new Rank(childRank);

      // Initially assume all stops are off
      mUnstopped.insert(Stop(childName, childLength));
    }


  } catch(ticpp::Exception & ex) {
    Env::errorMsg("Caught a ticpp exception in Division ctor %d", ex.what());
  } catch(...) {
    Env::errorMsg("Caught some other exception in Division ctor");
  }
}

Division::~Division() {
  std::map<Stop,Rank*>::iterator curRank;
  for(curRank = mRanks.begin(); curRank != mRanks.end();) {
    delete (*curRank).second;
    mRanks.erase(curRank++);
  }
  Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying Division named %s", mName.c_str());
}


int Division::sendEvent(const Input& in, int newValue) {
	
	switch(in.getType()) {

		case Input::ControlInput:
			if(in.getChannel() == mControlChannel) {
				if(in.getControlNumber() == MidiControlTypes_ExpressionPedal) {
					mExpressionValue = ((float)newValue) / ((float)mExpressionMax);
				}
				return 0;
			}
			break;

		case Input::SpecificInput:
			if(mStops.find(in) != mStops.end()) {
				if(newValue == 1) {
					mUnstopped.erase(mStops[in]);
					return 1;
				} else if(newValue == 0) {
					mUnstopped.insert(mStops[in]);
					return 1;
				}
				return 0;
			}
			if(mEffects.find(in) != mEffects.end()) {
				if(newValue == 1) {
					mEffected.insert(mEffects[in]);
					return 1;
				} else if(newValue == 0) {
					mEffected.erase(mEffects[in]);
					return 1;
				}
			}
			break;
	}
	return 0;
}

int Division::keyboardStateChange(Keyboard* keyboard) {
	std::map<Keyboard*, int>::iterator iter = mCoupled.find(keyboard);
	if(iter == mCoupled.end()) {
		Env::logMsg(Env::OperationMsg, Env::Debug, "State change on keyboard: %s, but it is not coupled to division: %s", keyboard->getName().c_str(), mName.c_str());
		return 0;
	}

	Env::logMsg(Env::OperationMsg, Env::Debug, "State change on keyboard: %s coupled to division: %s", keyboard->getName().c_str(), mName.c_str());

	return 0;	
}
