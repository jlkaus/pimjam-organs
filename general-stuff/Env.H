#ifndef _ENV_H
#define _ENV_H

#include <iostream>
#include <string>
#include <execinfo.h>

class Env {
	public:
	enum MsgClass {
		CreationMsg,
		OperationMsg
	};

	enum EnvLoudness {
		Debug = 30,
		Info = 20,
		Error = 10,
		None = 0
	};

	enum EnvIndentValue {
		OrganIndent = 0,
		DivisionIndent = 2,
		CouplerIndent = 4,
		EventIndent = 4,
		InputIndent = 4,
		KeyboardIndent = 4,
		EffectIndent = 4,
		StopIndent = 4,
		EventGeneratorIndent = 4,
		RankIndent = 6,
		PipeIndent = 10
	};

	Env(int cl, int ol): mDisplayCurrently(true),mCreationLoudness(cl), mOperationLoudness(ol) {}
	~Env() {}

	static void setLoudness(int cl, int ol) {
	       gEnv.mCreationLoudness = cl;
	       gEnv.mOperationLoudness = ol;
	}

	static int getCreationLoudness() { return gEnv.mCreationLoudness; }
	static int getOperationLoudness() { return gEnv.mOperationLoudness; }
	
  static Env& msg(MsgClass mc, int ml = 0, int indent=0) {
		if((mc == CreationMsg && ml <= gEnv.mCreationLoudness) ||
		(mc == OperationMsg && ml <= gEnv.mOperationLoudness)) {
			gEnv.mDisplayCurrently = true;
			for(int i=0;i<indent;++i) {
			  std::cout << " ";
			}
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

	static Env& msg() {
		gEnv.mDisplayCurrently = true;
		std::cout << "MSG: ";
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

	Env& operator<<(std::ostream& (*fn)(std::ostream&)) {
		if(mDisplayCurrently) {
		    fn(std::cout);
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

