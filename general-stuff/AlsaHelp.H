#ifndef _ALSAHELP_H
#define _ALSAHELP_H

#include <stdint.h>
#include <stddef.h>
#include <alsa/asoundlib.h>

struct frame_t {
  int16_t left;
  int16_t right;
};

snd_pcm_t* prepAlsa(const char* device, snd_pcm_uframes_t &bsize, snd_pcm_uframes_t &psize);



#endif
