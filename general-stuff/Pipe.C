#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Pipe.H"

Pipe::Pipe(FILE* file, int offset) {
  std::cout << "\t\t\t\t\tReading in Pipe data from an already open file" << std::endl;
}

Pipe::Pipe(std::string pipeFile) {
  std::cout << "\t\t\t\t\tCreating a Pipe from data in file "<< pipeFile << std::endl;
}
  
Pipe::~Pipe() {

}
