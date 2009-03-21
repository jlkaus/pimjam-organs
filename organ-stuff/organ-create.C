#include <iostream>
#include "Organ.H"
#include "Env.H"

int main(int argc, char* argv[]) {

  Env::setLoudness(1,1);
  Env::msg() << "Starting the organ creation." <<std::endl;
  
  Organ pimjam("/home/jlkaus/code/organs-trunk/pimjam.organ");
  
  Env::msg() << "Exiting main now.  Dtors to follow I suppose." << std::endl;

}
