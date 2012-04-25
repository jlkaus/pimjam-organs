#include "Organ.H"
#include "EventHandler.H"
#include "SComm.H"
#include "Env.H"

int main(int argc, char* argv[])
{
	//Env::setLoudness(99,99);
	Env::setLoudness(Env::Info, Env::Info);

	Organ pimjam("../pimjam.organ");

	// Start the event handler processing events
	EventHandler main_handler(&pimjam);
	main_handler.start();

	// Attach a new event generator to the event handler
	SComm scom_generator(&main_handler);
	scom_generator.startEvents();

	// Wait loop, wait for signals from user to quit, etc.
	while(1) {
		sleep(1);
	}
}
