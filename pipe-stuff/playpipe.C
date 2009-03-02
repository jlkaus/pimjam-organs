#include <sys/types.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "AlsaHelp.H"


struct pipe_data_t {
  time_t stop_time;
  int cur_index;
  snd_pcm_uframes_t buffer_size;
  snd_pcm_uframes_t period_size;
  snd_pcm_uframes_t samples;
  frame_t waveform[0];
};

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

      snd_pcm_t* handle = prepAlsa("hw:0,0",pipeData->buffer_size, pipeData->period_size);
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

