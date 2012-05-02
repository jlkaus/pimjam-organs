#include "EventGenerator.H"

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void EventGenerator::startEvents()
{
	if(xEventProcessingFunction == NULL) {
		fprintf(stderr, "Fatal error, event processing function is NULL\n");
		exit(1);
	}

	if(xProcessEvents) {
		stopEvents();
	}

	xProcessEvents = true;
	pthread_create(&xThreadId, NULL, xEventProcessingFunction, this);
}

void EventGenerator::stopEvents()
{
	if(!xProcessEvents) {
		return;
	}

	xProcessEvents = false;
	pthread_join(xThreadId, NULL);
}
