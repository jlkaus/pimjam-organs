#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

struct frame_t {
  int16_t left;
  int16_t right;
};

struct pipe_data_t {
  time_t stop_time;
  int cur_index;
  snd_pcm_uframes_t buffer_size;
  snd_pcm_uframes_t period_size;
  snd_pcm_uframes_t samples;
  frame_t waveform[0];
};

snd_pcm_t* prepAlsa(const char* device, pipe_date_t* pipeData) {
  int err;
  snd_pcm_t* handle=NULL;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_sw_params_t *sw_params;
  snd_pcm_uframes_t bsize = pipeData->buffer_size;
  snd_pcm_uframes_t psize = pipeData->period_size;
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
		if((err = snd_pcm_hw_params_set_buffer_size_near(handle, hw_params, &(pipeData->buffer_size),NULL))==0) {
		  if(buffer_size != bsize) {
		    fprintf(stderr,"*** buffer size set to %d instead of %d\n",pipeData->buffer_size, bsize);
		  }
		  if((err = snd_pcm_hw_params_set_period_size_near(handle, hw_params, &(pipeData->period_size),NULL))==0) {
		    if(period_size != psize) {
		      fprintf(stderr,"*** period size set to %d instead of %d\n",pipeData->period_size, psize);
		    }
		    if ((err = snd_pcm_hw_params (handle, hw_params))== 0) {
		      snd_pcm_hw_params_free (hw_params);
		      if((err = snd_pcm_sw_params_malloc(&sw_params))==0) {
			if((err = snd_pcm_sw_params_current(handle, sw_params))==0) {
			  if((err = snd_pcm_sw_params_set_start_threshold(handle, sw_params, pipeData->buffer_size - pipeData->period_size))==0) {
			    if((err = snd_pcm_sw_params_set_avail_min(handle, sw_params, pipeData->period_size))==0) {
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

pipe_data_t* prepPipe(const char* pipeFile, int buffer_size, int period_size) {
  pipe_data_t* pipeData = NULL;

  FILE* pipe_file = fopen(pipeFile);
  if(pipe_file) {
    snd_pcm_uframes_t samples;
    // compute file size in number of frames

    pipeData = malloc(sizeof(pipe_data_t) + samples*sizeof(frame_t) + period_size);
    if(pipeData) {
      pipeData->stop_time = 0;
      pipeData->cur_index = 0;
      pipeData->buffer_size = buffer_size;
      pipeData->period_size = period_size;
      pipeData->samples = samples;

      // read in data from pipefile and convert to int16 and copy to both channels of frame

      // copy first pcm period of data to the last frames of the waveform data array to account for wraparound effects

    } else {
      fprintf(stderr,"cannot allocate room for waveform data\n");
    }
    fclose(pipe_file);
  } else {
    fprintf(stderr,"cannot open pipe file %s\n",pipeFile);
  }
  return pipeData;
}

void runningLow(snd_async_handler_t *pcm_callback) {
  pipe_data_t* pipeData = snd_async_handler_get_callback_private(pcm_callback);
  snd_pcm_t* handle = snd_async_handler_get_pcm(pcm_callback);
  snd_pcm_sframes_t avail = snd_pcm_avail_update(handle);
  int err;

  while(avail >= pipeData->period_size) {
    if((err = snd_pcm_writei(handle, &pipeData->waveform[pipeData->cur_index], pipeData->period_size)) < 0) {
      fprintf(stderr, "*** underrun.  re-preparing. (%s)\n", snd_strerror(err));
      if((err = snd_pcm_prepare(handle)) < 0) {
	fprintf(stderr,"*** cannot prepare (%s)\n",snd_strerror(err));
	exit(-1);
      }
    }

    pipeData->cur_index+=pipeData->period_size/sizeof(frame_t);
    if(pipeData->cur_index >= pipeData->samples) {
      pipeData->cur_index-=pipeData->samples;
    }

    avail = snd_pcm_avail_update(handle);
  }
}

int main (int argc, char *argv[])
{
  // interpret arguments and open the pipe file to load it into memory, then close it.
  if(argc ==3) {
    pipe_date_t* pipeData = prepPipe(argv[1], 1024*sizeof(frame_t), 16*sizeof(frame_t));
    if(pipeData) {
      printf(": Pipe data loaded.\n");
      pipeData->stop_time = time() + atoi(argv[2]);

      snd_pcm_t* handle = prepAlsa("hw:0,0");
      if(handle) {
	printf(": ALSA set up.\n");
	snd_async_handler_t *pcm_callback;
	snd_async_add_pcm_handler(&pcm_callback, handle, runningLow, pipeData);

	// call the runningLow function to fill up the buffer to start with
	runningLow(pcm_callback);

	printf(": Starting to play pipe\n");
	snd_pcm_start(handle);

	while(time() < pipeData->stop_time) {
	  sleep(1);
	}

	printf(": Stop time reached\n");

	snd_pcm_drain(handle);

	snd_async_del_handler(pcm_callback);
	snd_pcm_close (handle);

	printf(": Cleaned up and exiting.\n");
      }
      delete pipe_data_t;
    }
  } else {
    fprintf(stderr,"not enough arguments.  syntax: playpipe <path to pipe file> <play duration in seconds>\n");
  }

  return (0);
}

