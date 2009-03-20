#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "AlsaHelp.H"


snd_pcm_t* prepAlsa(const char* device, snd_pcm_uframes_t &bsize, snd_pcm_uframes_t &psize) {
  int err;
  snd_pcm_t* handle=NULL;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_sw_params_t *sw_params;
  snd_pcm_uframes_t bsize_des = bsize;
  snd_pcm_uframes_t psize_des = psize;
  unsigned int rate=44100;

  if ((err = snd_pcm_open (&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) == 0) {
    if ((err = snd_pcm_hw_params_malloc (&hw_params))== 0) {
      if ((err = snd_pcm_hw_params_any (handle, hw_params))== 0) {
	if ((err = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED))== 0) {
	  if ((err = snd_pcm_hw_params_set_format (handle, hw_params, SND_PCM_FORMAT_S16_LE))== 0) {
	    if ((err = snd_pcm_hw_params_set_rate_near (handle, hw_params, &rate, 0))== 0) {
	      if(rate != 44100) {
		fprintf(stderr,"*** sample rate set to %d instead of %d\n",rate,44100);
	      }
	      if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, 2))== 0) {
		if((err = snd_pcm_hw_params_set_buffer_size_near(handle, hw_params, &bsize))==0) {
		  if(bsize != bsize_des) {
		    fprintf(stderr,"*** buffer size set to %d instead of %d\n",bsize, bsize_des);
		  }
		  if((err = snd_pcm_hw_params_set_period_size_near(handle, hw_params, &psize,0))==0) {
		    if(psize != psize_des) {
		      fprintf(stderr,"*** period size set to %d instead of %d\n",psize, psize_des);
		    }
		    if ((err = snd_pcm_hw_params (handle, hw_params))== 0) {
		      snd_pcm_hw_params_free (hw_params);
		      if((err = snd_pcm_sw_params_malloc(&sw_params))==0) {
			if((err = snd_pcm_sw_params_current(handle, sw_params))==0) {
			  if((err = snd_pcm_sw_params_set_start_threshold(handle, sw_params, bsize - psize))==0) {
			    if((err = snd_pcm_sw_params_set_avail_min(handle, sw_params, psize))==0) {
			      if((err = snd_pcm_sw_params(handle, sw_params))==0) {
				snd_pcm_sw_params_free(sw_params);
				if ((err = snd_pcm_prepare (handle))== 0) {
				  // all good!
				} else {
				  fprintf (stderr, "cannot prepare audio interface for use (%s)\n",snd_strerror (err));
				}
			      } else {
				fprintf(stderr,"cannot set sw parameters (%s)\n",snd_strerror(err));
			      }
			    } else {
			      fprintf(stderr,"cannot set interrupt threshold (%s)\n",snd_strerror(err));
			    }
			  } else {
			    fprintf(stderr,"cannot set start threshold (%s)\n",snd_strerror(err));
			  }
			} else {
			  fprintf(stderr,"cannot initialize sw params structure (%s)\n",snd_strerror(err));
			}
		      } else {
			fprintf(stderr, "cannot allocate sw params (%s)\n",snd_strerror(err));
		      }
		    } else {
		      fprintf (stderr, "cannot set hw parameters (%s)\n",snd_strerror (err));
		    }
		  } else {
		    fprintf(stderr, "cannot set period size (%s)\n",snd_strerror(err));
		  }
		} else {
		  fprintf(stderr, "cannot set buffer size (%s)\n",snd_strerror(err));
		}
	      } else {
		fprintf(stderr, "cannot set channel count (%s)\n",snd_strerror (err));
	      }
	    } else {
	      fprintf(stderr,"cannot set sample rate (%s)\n",snd_strerror(err));
	    }
	  } else {
	    fprintf(stderr,"cannot set sample format (%s)\n",snd_strerror(err));
	  }
	} else {
	  fprintf (stderr, "cannot set access type (%s)\n",snd_strerror (err));
	}
      } else {
	fprintf(stderr,"cannot initialize hardware parameter structure (%s)\n",snd_strerror(err));
      }
    } else {
      fprintf(stderr,"cannot allocate hardware parameter structure (%s)\n",snd_strerror(err));
    }
    if(err != 0) {
      snd_pcm_close(handle);
      handle = NULL;
    }
  } else {
    fprintf(stderr,"cannot open audio device %s (%s)\n", device, snd_strerror (err));
    handle = NULL;
  }

  return handle;
}

