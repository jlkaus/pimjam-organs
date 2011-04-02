#include <pthread.h>
#include <errno.h>
#include "Division.H"
#include "DivisionThread.H"

DivisionThread::DivisionThread(Division* division) :
	xThreadRunning(false),
	xThreadId(0),
	xDivision(division)
{
	Env::logMsg(Env::CreationMsg, Env::Debug, "Creating division thread");
	if(-1 == sem_init(&xEventSemaphore, 0, 0)) {
		Env::errorMsg("Division thread unable to initialize event semaphore: %s", strerror(errno));
		throw 10;
	}
}


DivisionThread::~DivisionThread()
{
	stopThread();
	
	if(-1 == sem_destroy(&xEventSemaphore)) {
		Env::errorMsg("Event handler unable to destory event semaphore: %s", strerror(errno));
		throw 10;
	}
}

void DivisionThread::startThread()
{
	if(xThreadRunning) {
		stopThread();
	}

	xThreadRunning = true;
	pthread_create(&xThreadId, NULL, &eventLoop, this);
}

void DivisionThread::stopThread()
{
	if(!xThreadRunning) {
		return;
	}

	xThreadRunning = false;
	sem_post(&xEventSemaphore);
	pthread_join(xThreadId, NULL);
}

void DivisionThread::notifyOfStateChange()
{	
	sem_post(&xEventSemaphore);
}

void* DivisionThread::eventLoop(void* arg)
{
	DivisionThread* thisThread = (DivisionThread*)arg;
	timespec wait_time = { 0, 100000000 };
	int sem_return_code = 0;

	while(thisThread->xThreadRunning) {

		if(sem_return_code != -1) {
			// Woke up by sem_post rescan division state
			
		}
	       
		// Fill sample buffer

		sem_return_code = sem_timedwait(&thisThread->xEventSemaphore, &wait_time);
	}
	
	// Perform division thread cleanup
}
