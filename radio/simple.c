#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <fftw3.h>
#include <jack/jack.h>



static const double MIDDLE_C    = 261.6;
static const double SAMPLE_RATE = 44100;
static const int    BUFFER_SIZE = 44100;

static const char * const CLIENT_NAME = "simple";
static const char * const INPUT_NAME  = "input";
static const char * const OUTPUT_NAME = "output";
static const char * const SINE_NAME   = "sine";
static const char * const SQUARE_NAME = "square";



typedef jack_default_audio_sample_t jack_sample_t;



typedef struct
{
  int    index;
  double value;
}
pulse;



typedef struct
{
  double         frequency;
  double         sample_rate;

  jack_port_t   *input_port;
  jack_port_t   *output_port;
  jack_port_t   *sine_port;
  jack_port_t   *square_port;

  int            block_count;
  int            block_size;

  fftw_plan      fft_routine;

  jack_sample_t *sample_data;
  double        *window_data;
  double        *input_data;
  fftw_complex  *output_data;

  int            current_block;

  pulse         *indexed_train;
  pulse         *sorted_train;
}
state;



double window    (int             samples,
                  int             sample);

int    process   (jack_nframes_t  nframes,
                  void           *arg);

int    compare   (const void     *first,
                  const void     *second);

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

  char *end = argv[1];

  if (argc > 1)
    data.frequency = strtod (argv[1], &end);

  if (end == argv[1])
    data.frequency = MIDDLE_C;

  data.sample_rate = jack_get_sample_rate (client);

  if (data.sample_rate < 1)
    data.sample_rate = SAMPLE_RATE;

  data.input_port = jack_port_register (client,
                                        INPUT_NAME,
                                        JACK_DEFAULT_AUDIO_TYPE,
                                        JackPortIsInput,
                                        0);

  data.output_port = jack_port_register (client,
                                         OUTPUT_NAME,
                                         JACK_DEFAULT_AUDIO_TYPE,
                                         JackPortIsOutput,
                                         0);

  data.sine_port = jack_port_register (client,
                                       SINE_NAME,
                                       JACK_DEFAULT_AUDIO_TYPE,
                                       JackPortIsOutput,
                                       0);

  data.square_port = jack_port_register (client,
                                         SQUARE_NAME,
                                         JACK_DEFAULT_AUDIO_TYPE,
                                         JackPortIsOutput,
                                         0);

  data.block_size  = jack_get_buffer_size (client);
  data.block_count = BUFFER_SIZE / data.block_size;

  int buffer_size = data.block_count * data.block_size;

  data.sample_data = calloc (2 * buffer_size - data.block_size, sizeof (jack_sample_t));
  data.window_data = malloc (buffer_size * sizeof (double));
  data.input_data  = fftw_malloc (buffer_size * sizeof (double));
  data.output_data = fftw_malloc ((buffer_size / 2 + 1) * sizeof (fftw_complex));

  data.fft_routine = fftw_plan_dft_r2c_1d (buffer_size,
                                           data.input_data,
                                           data.output_data,
                                           FFTW_MEASURE | FFTW_DESTROY_INPUT);

  data.indexed_train = calloc (buffer_size / 2 + 1, sizeof (pulse));
  data.sorted_train  = malloc ((buffer_size / 2 + 1) * sizeof (pulse));

  data.current_block = 0;

  int sample;

  for (sample = 0; sample < buffer_size; sample++)
    data.window_data[sample] = window (buffer_size, sample);

  for (sample = 0; sample < buffer_size / 2 + 1; sample++)
    data.indexed_train[sample].index = sample;

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

  jack_sample_t *input_buffer  = jack_port_get_buffer (data->input_port,  nframes);
  jack_sample_t *output_buffer = jack_port_get_buffer (data->output_port, nframes);
  jack_sample_t *sine_buffer   = jack_port_get_buffer (data->sine_port,   nframes);
  jack_sample_t *square_buffer = jack_port_get_buffer (data->square_port, nframes);

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

  memcpy (data->sorted_train, data->indexed_train, (buffer_size / 2 + 1) * sizeof (pulse));

  for (sample = 0; sample < buffer_size / 2 + 1; sample++)
    data->sorted_train[sample].value = magnitude (data->output_data[sample]);

  qsort (data->sorted_train, buffer_size / 2 + 1, sizeof (pulse), compare);

  double alpha = 2 * M_PI * data->sorted_train[buffer_size / 2].index / buffer_size;
  double omega = 2 * M_PI * data->frequency / data->sample_rate;

  for (sample = 0; sample < nframes; sample++)
  {
    int current_sample = data->current_block * nframes + sample;

    output_buffer[sample] = sin (alpha * current_sample);
    sine_buffer[sample]   = sin (omega * current_sample);
    square_buffer[sample] = sine_buffer[sample] < 0 ? -1 : 1;
  }

  data->current_block++;

  return 0;
}



int
compare (const void *first,
         const void *second)
{
  const pulse *first_pulse  = first;
  const pulse *second_pulse = second;

  double first_value  = first_pulse->value;
  double second_value = second_pulse->value;

  return first_value < second_value ? -1 : first_value == second_value ? 0 : 1;
}



double
magnitude (fftw_complex number)
{
  return sqrt (number[0] * number[0] + number[1] * number[1]);
}
