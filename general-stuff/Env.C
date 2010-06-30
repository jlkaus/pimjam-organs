#include "Env.H"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <execinfo.h>

Env Env::gEnv(0,0);

void Env::logMsg(MsgClass mc, int ml, const char* fmt, ...) {

	if((mc == CreationMsg && ml <= gEnv.mCreationLoudness) ||
		(mc == OperationMsg && ml <= gEnv.mOperationLoudness)) {

		const char* msg_title = "";
		bool print_symbol = false;
		if(mc == CreationMsg) {
			msg_title = "CRT";
			if(gEnv.mCreationLoudness >= Env::Debug) {
				print_symbol = true;
			}
		} else if(mc == OperationMsg) {
			if(gEnv.mOperationLoudness >= Env::Debug) {
				print_symbol = true;
			}
			msg_title = "OPR";
		} else {
			msg_title = "???";
		}

		va_list argList;
		va_start(argList, fmt);
		printCommon(stdout, msg_title, print_symbol, fmt, argList);	
		va_end(argList);
        }
}

void Env::errorMsg(const char* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	printCommon(stderr, "ERR", true, fmt, argList);
	va_end(argList);
}

void Env::msg(const char* fmt, ...)
{
	va_list argList;
	va_start(argList, fmt);
	printCommon(stdout, "MSG", false, fmt, argList);
	va_end(argList);
}

void Env::printCommon(FILE* stream, const char* msg_title, bool print_symbol, const char* fmt, va_list ap)
{
	void* backtrace_stack[20];
	int backtrace_stack_size = backtrace(backtrace_stack, 20);


        time_t rawtime;
        struct tm* timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        char timebuffer[30];
        strftime(timebuffer, sizeof(timebuffer), "%c", timeinfo);

        fprintf(stream, "[ %s ] ", timebuffer);

	// Indent based on call stack size
	//   subtract 2 for the logMsg/errorMsg call and the printCommon call
	for(int i = 0; i < backtrace_stack_size - 2; ++i) {
		fprintf(stream, " ");
	}
	
	fprintf(stream, "%s: ", msg_title);
	
	if(print_symbol) {	
		// Use symbol in 2nd entry since logMsg/errorMsg and printCommon are on the callstack
		char** symbols = backtrace_symbols(backtrace_stack, backtrace_stack_size);
		fprintf(stream, "%s ", symbols[2]);
		free(symbols);
	}

        vfprintf(stream, fmt, ap);
        fprintf(stream, "\n");
}
