#include <map>
#include <iostream>
#include <set>
#include "ticpp.h"

#include "Division.H"
#include "Rank.H"
#include "Input.H"
#include "AlsaPlayControlBlock.H"


Division::Division(ticpp::Element* divisionDescription) {
  try {
    mName = divisionDescription->GetAttribute("name");
    std::cout << "\tBuilding Division of name: "<<mName << std::endl;

    ticpp::Iterator<ticpp::Element> childK("keyboard");
    for(childK = childK.begin(divisionDescription); childK!= childK.end(); ++childK) {
      std::string childName = childK->GetAttribute("name");
      int childChannel = -1;
      childK->GetAttributeOrDefault("channel", &childChannel,-1);
      int childOffset = 0;
      childK->GetAttributeOrDefault("length",&childOffset,0);
			
      std::cout << "\t\tAdding Keyboard of name "<<childName<<" with offset "<<childOffset<<" on channel "<<childChannel<<std::endl;
			
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
			
      std::cout << "\t\tAdding Coupler of name "<<childName<<" with target of "<<childTarget << ", offset "<<childOffset<<" on channel "<<childChannel<<", input " << childInput <<std::endl;
			
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
			
      std::cout << "\t\tAdding Effect of name "<<childName<<" with type of "<<childType << ", arg "<<childArg<<" on channel "<<childChannel<<", input " << childInput <<std::endl;
			
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
			
      std::cout << "\t\tAdding Stop of name "<<childName<<" with rank of "<<childRank << ", length "<<childLength<<" on channel "<<childChannel<<", input " << childInput <<std::endl;
			
      mStops[Input(childChannel,childInput)] = Stop(childName, childLength);
      
      std::cout << "\t\tCreating rank from data in " << childRank << std::endl;

      mRanks[Stop(childName, childLength)] = new Rank(childRank);
    }


  } catch(ticpp::Exception & ex) {
    std::cout << "*** Something horrible happened: "<< ex.what() << std::endl;
  } catch(...) {
    std::cout << "*** Got something I don't understand" << std::endl;
  }
}

Division::~Division() {
  std::map<Stop,Rank*>::iterator curRank;
  for(curRank = mRanks.begin(); curRank != mRanks.end();) {
    delete (*curRank).second;
    mRanks.erase(curRank++);
  }
}


int Division::sendEvent(const Input& in, AlsaPlayControlBlock& apcb) {

}
