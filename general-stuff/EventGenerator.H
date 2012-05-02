#ifndef _EVENTGENERATOR_H
#define _EVENTGENERATOR_H

#include <pthread.h>

class EventHandler;

typedef void* (EventProcessingFunction)(void*);

class EventGenerator {

	public:
		void startEvents();
		void stopEvents();

	protected:
		EventGenerator(EventHandler* event_handler, EventProcessingFunction* process_function) : xEventHandler(event_handler), xProcessEvents(false), xThreadId(-1), xEventProcessingFunction(process_function) { }
		
		EventHandler* xEventHandler;
		bool xProcessEvents;
		pthread_t xThreadId;
		EventProcessingFunction* xEventProcessingFunction;
};

#endif
