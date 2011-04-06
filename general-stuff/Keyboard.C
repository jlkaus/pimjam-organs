#include "Keyboard.H"
#include "Division.H"
#include "Event.H"

Event::EventStatus Keyboard::sendEvent(const Input& in, int newValue) {

	if(in.getChannel() != mChannel) {
		return Event::EventUnhandled;
	}

	if(newValue == 0) {
		mPressedKeys.erase(in.getLine());
		notifyDivisionsOfStateChange();
		return Event::EventConsumed;
	}

	if(newValue = 1) {
		mPressedKeys.insert(in.getLine());
		notifyDivisionsOfStateChange();
		return Event::EventConsumed;
	}

	return Event::EventUnhandled;
}


void Keyboard::notifyDivisionsOfStateChange() {

	if(Env::getOperationLoudness() >= Env::Debug) {
		std::string pressed_keys;
		for(std::set<int>::iterator iter = mPressedKeys.begin(); iter != mPressedKeys.end(); iter++) {
			char key_string[10];
			snprintf(key_string, sizeof(key_string), "%d ", *iter);
			pressed_keys.append(key_string);
		}
		Env::logMsg(Env::OperationMsg, Env::Debug, "Keyboard: %s Pressed Keys: %s", mName.c_str(), pressed_keys.c_str());
	}

	for(std::set<Division*>::iterator iter = mCoupledDivisions.begin(); iter != mCoupledDivisions.end(); iter++) {
		(*iter)->keyboardStateChange(this);
	}
}
