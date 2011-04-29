#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include <fftw3.h>
#include <jack/jack.h>



static const double SAMPLE_RATE = 44100;
static const int    BUFFER_SIZE = 44100;

static const char * const OUTPUT_FILE = "recording";
static const char * const CLIENT_NAME = "record";
static const char * const INPUT_NAME  = "input";



typedef jack_default_audio_sample_t jack_sample_t;



typedef struct
{
  FILE          *output_file;

  double         sample_rate;

  jack_port_t   *input_port;

  int            block_count;
  int            block_size;

  fftw_plan      fft_routine;

  jack_sample_t *sample_data;
  double        *window_data;
  double        *input_data;
  fftw_complex  *output_data;
  float         *vector_data;

  int            current_block;
}
state;



double window    (int             samples,
                  int             sample);

int    process   (jack_nframes_t  nframes,
                  void           *arg);

double magnitude (fftw_complex    number);



double
window (int samples,
        int sample)
{
  return 0.5 - 0.5 * cos (2 * M_PI * sample / samples);
}



int
main (int   argc,
      char *argv[])
{
  jack_client_t *client = jack_client_open (CLIENT_NAME,
                                            JackNullOption,
                                            NULL);

  state data;

  data.output_file = fopen (argc > 1 ? argv[1] : OUTPUT_FILE, "wb");

  data.sample_rate = jack_get_sample_rate (client);

  if (data.sample_rate < 1)
    data.sample_rate = SAMPLE_RATE;

  data.input_port = jack_port_register (client,
                                        INPUT_NAME,
                                        JACK_DEFAULT_AUDIO_TYPE,
                                        JackPortIsInput,
                                        0);

  data.block_size  = jack_get_buffer_size (client);
  data.block_count = BUFFER_SIZE / data.block_size;

  int buffer_size = data.block_count * data.block_size;

  data.sample_data = calloc (2 * buffer_size - data.block_size, sizeof (jack_sample_t));
  data.window_data = malloc (buffer_size * sizeof (double));
  data.input_data  = fftw_malloc (buffer_size * sizeof (double));
  data.output_data = fftw_malloc ((buffer_size / 2 + 1) * sizeof (fftw_complex));
  data.vector_data = malloc ((buffer_size / 2 + 1) * sizeof (float));

  data.fft_routine = fftw_plan_dft_r2c_1d (buffer_size,
                                           data.input_data,
                                           data.output_data,
                                           FFTW_MEASURE | FFTW_DESTROY_INPUT);

  data.current_block = 0;

  int sample;

  for (sample = 0; sample < buffer_size; sample++)
    data.window_data[sample] = window (buffer_size, sample);

  jack_set_process_callback (client, process, &data);

  jack_activate (client);

  while (pause ());

  return 0;
}



int
process (jack_nframes_t  nframes,
         void           *arg)
{
  state *data = arg;

  if (nframes != data->block_size)
    return -1;

  jack_sample_t *input_buffer = jack_port_get_buffer (data->input_port,  nframes);

  int current_block = data->current_block % data->block_count;
  int buffer_size   = data->block_count * nframes;
  int buffer_offset = current_block * nframes;

  memcpy (data->sample_data + buffer_size + buffer_offset - nframes,
          input_buffer,
          nframes * sizeof (jack_sample_t));

  if (current_block)
    memcpy (data->sample_data + buffer_offset - nframes,
            input_buffer,
            nframes * sizeof (jack_sample_t));

  int sample;

  for (sample = 0; sample < buffer_size; sample++)
    data->input_data[sample] = data->sample_data[buffer_offset + sample] * data->window_data[sample];

  fftw_execute (data->fft_routine);

  int nonzero = 0;

  for (sample = 0; sample < buffer_size / 2 + 1; sample++)
  {
    data->vector_data[sample] = magnitude (data->output_data[sample]);

    nonzero |= data->vector_data[sample] > 1E-1;
  }

  if (nonzero)
  {
    fwrite (data->vector_data, sizeof (float), buffer_size / 2 + 1, data->output_file);
    fflush (data->output_file);
  }

  data->current_block++;

  return 0;
}



double
magnitude (fftw_complex number)
{
  return sqrt (number[0] * number[0] + number[1] * number[1]);
}
