#ifndef _EVENTHANDLER_H
#define _EVENTHANDLER_H

#include <queue>
#include <semaphore.h>
#include <pthread.h>
#include "Event.H"

class Organ;
class EventGenerator;

class EventHandler {

	public:
		EventHandler(Organ* thisOrgan);
		~EventHandler();

		void enqueueEvent(const Event& event);

		void start();
		void stop();

	private:

		static void* eventLoop(void* arg);
		
		Organ* xOrgan;
		EventGenerator* xEventGenerator;
		std::queue<Event> xEventQueue;
		sem_t xEventSemaphore;
		pthread_t xThreadId;
		bool xHandleEvents;
};

#endif
