#ifndef _PIPESET_H
#define _PIPESET_H

#include <vector>
#include <stdint.h>
#include <stdio.h>
#include "Pipe.H"
#include "rankfile.H"

class PipeSet {
public:
  PipeSet(FILE* rankFile, long int offset, int numPipes);
  PipeSet(std::string files[], int numPipes);
  ~PipeSet();
  


protected:
  int mWrittenNote;
  std::vector<Pipe> mPipes;


};


#endif

