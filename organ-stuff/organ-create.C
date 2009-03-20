#include <iostream>
#include "Organ.H"
#include "Env.H"

int main(int argc, char* argv[]) {

	Env::setLoudness(5,5);
	Env::msg(Env::OperationMsg, 3) << "Testing my msg output" << std::endl;
	Env::msg(Env::CreationMsg, 10) << "Testing my not outputing stuff" << std::endl;
	Env::wrn() << "Warning." <<std::endl;
	Env::err() << "Oh my gosh." <<std::endl;
	
  Organ pimjam("/home/jlkaus/code/organs-trunk/pimjam.organ");

  Env::msg() << "And done " <<std::endl;

}
