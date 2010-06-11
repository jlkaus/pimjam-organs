#ifndef _ENV_H
#define _ENV_H

#include <stdio.h>

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

	Env(int cl, int ol) : mCreationLoudness(cl), mOperationLoudness(ol) {}
	~Env() {}

	static void setLoudness(int cl, int ol) {
	       gEnv.mCreationLoudness = cl;
	       gEnv.mOperationLoudness = ol;
	}

	static int getCreationLoudness() { return gEnv.mCreationLoudness; }
	static int getOperationLoudness() { return gEnv.mOperationLoudness; }
	
	static void logMsg(MsgClass mc, int ml, const char* fmt, ...);
	static void errorMsg(const char* fmt, ...);
	static void msg(const char* fmt, ...);
	
	protected:

	static void printCommon(FILE* stream, const char* msg_title, bool print_symbol, const char* fmt, va_list ap);

	static Env gEnv;	
	int mCreationLoudness;
	int mOperationLoudness;
};

#endif

