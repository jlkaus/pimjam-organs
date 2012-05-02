#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

#include "Env.H"
#include "Event.H"
#include "Organ.H"
#include "EventHandler.H"

EventHandler::EventHandler(Organ* thisOrgan) :
	xOrgan(thisOrgan),
	xHandleEvents(false)
{
	Env::logMsg(Env::CreationMsg, Env::Debug, "Creating event generator");
	if(-1 == sem_init(&xEventSemaphore, 0, 0)) {
		Env::errorMsg("Event handler unable to initialize event semaphore: %s", strerror(errno));
		throw 10; 
	}
}

EventHandler::~EventHandler()
{
	Env::logMsg(Env::CreationMsg, Env::Debug, "Destroying event generator");
	if(xHandleEvents) {
		stop();
	}

	if(-1 == sem_destroy(&xEventSemaphore)) {
		Env::errorMsg("Event handler unable to destory event semaphore: %s", strerror(errno));
		throw 10;
	}
}

void EventHandler::enqueueEvent(const Event& event) 
{
	xEventQueue.push(event);
	if(-1 == sem_post(&xEventSemaphore)) {
		Env::errorMsg("Event handler unable to post to event semaphore: %s", strerror(errno));
		throw 10;
	}
}

void* EventHandler::eventLoop(void* args)
{
	EventHandler* thisHandler = (EventHandler*)args;

	while(thisHandler->xHandleEvents) {
		if(-1 == sem_wait(&thisHandler->xEventSemaphore)) {
			Env::errorMsg("Event handler error waiting on semaphore: %s", strerror(errno));
			throw 10;
		}

		Event this_event = thisHandler->xEventQueue.front();
		thisHandler->xEventQueue.pop();

		thisHandler->xOrgan->sendEvent(this_event.getInput(), this_event.getValue());
	}
}

void EventHandler::start()
{
	if(xHandleEvents) {
		stop();
	}

	xHandleEvents = true;
	pthread_create(&xThreadId, NULL, eventLoop, this);
}

void EventHandler::stop()
{
	if(!xHandleEvents) {
		return;
	}

	xHandleEvents = false;
	pthread_join(xThreadId, NULL);
}
