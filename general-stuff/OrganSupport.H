#ifndef ORGANSUPPORT_H
#define ORGANSUPPORT_H

class OrganSupport;

class OrganSupport {

	public:

		enum LogVerbosity {
			Debug = 3,
			Info = 2,
			Error = 1,
			None = 0
		};

		static int create(LogVerbosity verbosity);
		static inline void done() { if(xOrganSupport) { delete xOrganSupport; xOrganSupport = NULL; } }

		static inline LogVerbosity getVerbosityLevel() { return xOrganSupport?xOrganSupport->xVerbosity:None; }

		static void logMsg(LogVerbosity msg_verbosity, const char* fmt, ...);
		static void errorMsg(const char* fmt, ...);

	private:

		OrganSupport(LogVerbosity verbosity) : xVerbosity(verbosity) {}
		~OrganSupport() {}

		static OrganSupport* xOrganSupport;
		LogVerbosity xVerbosity;

};

#endif
