#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "Pipe.H"
#include "Env.H"

Pipe::Pipe(FILE* file, int offset) {
  Env::msg(Env::CreationMsg,10,10)<<"Creating pipe from already open file" << std::endl;

  Env::msg(Env::OperationMsg,1,10)<<"Loading pipe:"<<std::endl;
}

Pipe::Pipe(std::string pipeFile) {
  Env::msg(Env::CreationMsg,10,10)<<"Creating pipe from a file " <<pipeFile<< std::endl;
}
  
Pipe::~Pipe() {
  Env::msg(Env::CreationMsg,10,10)<<"Destroying pipe"<<std::endl;
}
