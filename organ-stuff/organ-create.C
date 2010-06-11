#include <iostream>
#include "Organ.H"
#include "Env.H"

int main(int argc, char* argv[]) {

  Env::setLoudness(99,99);
  Env::msg("Starting the organ creation.");
  
  Organ pimjam("../pimjam.organ");
  
  Env::msg("Exiting main now.  Dtors to follow I suppose.");

}
