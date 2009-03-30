#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "rankfile.H"


int main(int argc, char* argv[]) {
  // generate all dfts.
  // parms:	max mutation harmonic (9)
  //		detuning amount in cents (2)
  //		note range, in octave numbers (0-9)
  //		imperfection of stopping (0.25)
  //            imperfection of overblowing (0.25)
  //		decay rate step (0.50)
  //		decay rate max (2.00)
  //		limitation imperfection (0.25)
  //		list of limitation harmonics to generate other than fake ones for L0p and L0i

  // will place all dfts into directories based on pipe family (about 90 directories, given defaults. decay rate parameter changes might generate more): Xddxxyyz
  // So each directoy will contain all dfts for that family, including all colors, all mutations, all notes, and all tunings.  Given defaults, about 20,000 dfts per directory.

  return 0;
}

