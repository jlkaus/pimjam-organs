#include "Division.H"
#include "Input.H"
#include "AlsaPlayControlBlock.H"
#include <map>
#include <ostream>
#include "ticpp.h"
#include "Rank.H"
#include <set>

Division::Division(ticpp::Element* divisionDescription) {
	try {
		mName = divisionDescription->GetAttribute("name");
		std::cout << "\tBuilding Division of name: "<<mName << std::endl;

		ticpp::Iterator<ticpp::Element> child("keyboard");
		for(child = child.begin(divisionDescription); child!= child.end(); ++child) {
			std::string childName = child->GetAttribute("name");
			int childChannel = child->GetAttribute("channel");
			int childOffset = child->GetAttribute("length",FALSE);
			
			std::cout << "\t\tAdding Keyboard of name "<<childName<<" with offset "<<childOffset<<" on channel "<<childChannel<<std::endl;
			
			mKeyboards[Input(childChannel)] = Keyboard(childName, childOffset);
		}



	} catch(ticpp::Exception & ex) {
		std::cout << "*** Something horrible happened: "<< ex.What() << std::endl;
	}
}

Division::~Division() {
	std::map<Stop,Rank*>::iterator curRank;
	for(curRank = mRanks.begin(); curRank != mRanks.end(); ++curRank) {
		delete curRank.second;
		mRanks.erase(curRank);
	}
}


int Division::sendEvent(const Input& in, AlsaPlayControlBlock& apcb) {

}
