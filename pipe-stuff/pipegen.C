#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "rankfile.H"




int main(int argc, char* argv[]) {
  // ok, args for pipegen are just the spectrum file to transform.
  // interpret argv[1] to get the rank name and the fundamental
  // then use it to decide what the pipe file name should be

  char *spectrum_fn = argv[1];
  char *pipe_fn = (char*)alloca(strlen(spectrum_fn)+2);
  bzero(pipe_fn, strlen(spectrum_fn)+2);
  strcpy(pipe_fn, spectrum_fn);
  int fundamental = 0;
  char *tex = pipe_fn + strlen(spectrum_fn) - 4;
  *tex=0;
  char *tus = tex;
  while(*(--tus) != '_'); 
  fundamental = atoi(tus+1);
  strcpy(tex,".pipe");

  printf("Spectrum file: %s\n",spectrum_fn);
  printf("Pipe file: %s\n",pipe_fn);
  printf("Fundamental: %d\n",fundamental);


  // create the pipe header structure

  // open the spectrum file
  // open and create the pipe file

  // perform a DFT on the spectrum data and write the data out to the pipe file following the header

  // close both files

}
