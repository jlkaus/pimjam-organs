#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

#include "PipeSet.H"
#include "Pipe.H"
#include "rankfile.H"

PipeSet::PipeSet(FILE* rankFile, long int offset, int numPipes) {
  std::cout << "\t\t\t\tCreating a PipeSet, I guess, from an already open file." << std::endl;
}

PipeSet::PipeSet(std::string files[], int numPipes) {
  std::cout << "\t\t\t\tCreating a PipeSet from an array of " << numPipes << " files." << std::endl;
}

PipeSet::~PipeSet() {

}

