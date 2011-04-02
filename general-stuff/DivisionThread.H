#ifndef _DIVISIONTHREAD_H
#define _DIVISIONTHREAD_H

#include <pthread.h>
#include <semaphore.h>

class Division;

class DivisionThread {

	public:
		DivisionThread(Division* division);
		~DivisionThread();

		void startThread();
		void stopThread();
		void notifyOfStateChange();

	private:
		Division* xDivision;
		
		pthread_t xThreadId;
		sem_t xEventSemaphore;
		bool xThreadRunning;
		static void* eventLoop(void *arg);

};

#endif
