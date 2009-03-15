#include "Organ.H"
#include <string>
#include "Input.H"
#include "AlsaPlayControlBlock.H"

Organ::Organ(std::string organFile) {
  // open the organ file
  // parse it as xml
  // creating a Division for each division section of the file and passing that element in the constructor of the Division.
  // close the organ file
}

Organ::~Organ() {
  // delete all the Divisions
}

int Organ::sendInput(const Input& in, AlsaPlayControlBlock& apcb) {
  // go through all the Divisions until one of them handles the Input
}

