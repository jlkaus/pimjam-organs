#include <map>
#include <iostream>
#include <set>
#include "ticpp.h"

#include "Division.H"
#include "Rank.H"
#include "Input.H"
#include "AlsaPlayControlBlock.H"
#include "Env.H"

Division::Division(ticpp::Element* divisionDescription) {
  try {
    mName = divisionDescription->GetAttribute("name");

    Env::msg(Env::CreationMsg,11,2) << "Creating division named " << mName << std::endl;

    Env::msg(Env::OperationMsg,1,2) << "Loading division: " << mName <<std::endl;

    ticpp::Iterator<ticpp::Element> childK("keyboard");
    for(childK = childK.begin(divisionDescription); childK!= childK.end(); ++childK) {
      std::string childName = childK->GetAttribute("name");
      int childChannel = -1;
      childK->GetAttributeOrDefault("channel", &childChannel,-1);
      int childOffset = 0;
      childK->GetAttributeOrDefault("length",&childOffset,0);
			
      mKeyboards[Input(childChannel)] = Keyboard(childName, childOffset);
    }

    ticpp::Iterator<ticpp::Element> childC("coupler");
    for(childC = childC.begin(divisionDescription); childC!= childC.end(); ++childC) {
      std::string childName = childC->GetAttribute("name");
      std::string childTarget = childC->GetAttribute("target");
      int childChannel = -1;
      childC->GetAttributeOrDefault("channel",&childChannel, -1);
      int childInput = -1;
      childC->GetAttributeOrDefault("input",&childInput,-1);
      int childOffset = 0;
      childC->GetAttributeOrDefault("length",&childOffset,0);
			
      mCouplers[Input(childChannel,childInput)] = Coupler(childName, childTarget, childOffset);
    }

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
    }


  } catch(ticpp::Exception & ex) {
    Env::err() << "Caught a ticpp exception in Division ctor " << ex.what() << std::endl;
  } catch(...) {
    Env::err() << "Caught some other exception in Division ctor" <<std::endl;
  }
}

Division::~Division() {
  std::map<Stop,Rank*>::iterator curRank;
  for(curRank = mRanks.begin(); curRank != mRanks.end();) {
    delete (*curRank).second;
    mRanks.erase(curRank++);
  }
  Env::msg(Env::CreationMsg, 11,2) << "Destroying Division named " << mName <<std::endl;
}


int Division::sendEvent(const Input& in, AlsaPlayControlBlock& apcb) {

}
