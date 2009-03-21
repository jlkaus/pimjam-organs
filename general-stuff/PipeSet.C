#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "PipeSet.H"
#include "Pipe.H"
#include "rankfile.H"
#include "Env.H"

PipeSet::PipeSet(FILE* rankFile, long int offset, int numPipes) {
  Env::msg(Env::CreationMsg,10,8) << "Creating PipeSet from already open file"<<std::endl;

  Env::msg(Env::OperationMsg,1,8) << "Loading PipeSet: "<<std::endl;
}

PipeSet::PipeSet(std::string files[], int numPipes) {
  Env::msg(Env::CreationMsg,10,8) << "Creating PipeSet from "<<numPipes<<" pipe files."<<std::endl;
}

PipeSet::~PipeSet() {
  Env::msg(Env::CreationMsg,10,8)<<"Destroying PipeSet"<<std::endl;
}

