#ifndef _PLAYCONTROLBLOCK_H
#define _PLAYCONTROLBLOCK_H

#include "Pipe.H"

class PlayControlBlock {
public:
  PlayControlBlock();
  ~PlayControlBlock();

  // adds the pipe to the list of pipes that are currently sounding, in attack mode, with sustain mode scheduled to begin after the attack is finished.
  int attackPipe(Pipe* pipe);

  // removes the pipe from the list of pipes that are currently in sustain mode, and put it into release mode, with it being entirely removed after that has finished.
  int releasePipe(Pipe* pipe);

protected:


};


#endif
