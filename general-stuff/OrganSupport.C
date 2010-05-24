#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "OrganSupport.H"

OrganSupport* OrganSupport::xOrganSupport = NULL;

int OrganSupport::create(LogVerbosity verbosity) 
{
	if(xOrganSupport == NULL) {
		xOrganSupport = new OrganSupport(verbosity);
		return xOrganSupport?1:0;
	} else {
		return -1;
	}
}

void OrganSupport::logMsg(LogVerbosity msg_verbosity, const char* fmt, ...) 
{
	if(msg_verbosity <= getVerbosityLevel()) {

		time_t rawtime;
		struct tm* timeinfo;
		
		time(&rawtime);
		timeinfo = localtime(&rawtime);
		//printf("[ %s ] ", asctime(timeinfo));

		va_list argList;
		va_start(argList, fmt);
		vprintf(fmt, argList);
		va_end(argList);
	}
}

void OrganSupport::errorMsg(const char* fmt, ...)
{
	va_list argList;
	
	va_start(argList, fmt);
	vfprintf(stderr, fmt, argList);
	va_end(argList);
	
	exit(1);
}
