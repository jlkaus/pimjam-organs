#ifndef _ENV_H
#define _ENV_H

#include <iostream>
#include <string>

class Env {
	public:
	enum MsgClass {
		CreationMsg,
		OperationMsg
	};

	Env(int cl, int ol): mDisplayCurrently(true),mCreationLoudness(cl), mOperationLoudness(ol) {}
	~Env() {}

	static Env& msg(MsgClass mc, int ml) {
		if((mc == CreationMsg && ml < gEnv.mCreationLoudness) ||
		(mc == OperationMsg && ml < gEnv.mOperationLoudness)) {
			gEnv.mDisplayCurrently = true;
			if(mc == CreationMsg) {
				std::cout << "CRT: ";
			} else if(mc == OperationMsg) {
				std::cout << "OPR: ";
			} else {
				std::cout << "???: ";
			}
		} else {
			gEnv.mDisplayCurrently = false;
		}
		return gEnv;
	}

	static Env& err() {
		gEnv.mDisplayCurrently = true;
		std::cout << "!!!: ";
		return gEnv;
	}

	static Env& wrn() {
		gEnv.mDisplayCurrently = true;
		std::cout << "***: ";
		return gEnv;
	}
	
	template<class T> Env& operator<<(T msg) {
		if(mDisplayCurrently) {
			std::cout << msg;
		}
		return *this;
	}


	protected:
	static Env gEnv;
	bool mDisplayCurrently;	
	int mCreationLoudness;
	int mOperationLoudness;
	
	
};


#endif

