#include <unistd.h>
#include <stdio.h>

#include <jack/jack.h>



static const char * const OUTPUT_FILE = "recording";
static const char * const CLIENT_NAME = "record";
static const char * const INPUT_NAME  = "input";

static const double ZERO = 1E-6;



typedef jack_default_audio_sample_t jack_sample_t;



typedef struct
{
  FILE        *output_file;

  jack_port_t *input_port;

  int          recording;
}
state;



int process (jack_nframes_t  nframes,
             void           *arg);



int
main (int   argc,
      char *argv[])
{
  jack_client_t *client = jack_client_open (CLIENT_NAME,
                                            JackNullOption,
                                            NULL);

  state data;

  data.output_file = fopen (argc > 1 ? argv[1] : OUTPUT_FILE, "wb");

  data.input_port = jack_port_register (client,
                                        INPUT_NAME,
                                        JACK_DEFAULT_AUDIO_TYPE,
                                        JackPortIsInput,
                                        0);

  data.recording = 0;

  int bytes = sizeof (jack_sample_t);

  fwrite (&bytes, sizeof (bytes), 1, data.output_file);
  fflush (data.output_file);

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

  jack_sample_t *input_buffer = jack_port_get_buffer (data->input_port, nframes);

  int sample = 0;

  data->recording = 0;

  for (sample = 0; !data->recording && sample < nframes; sample++)
    data->recording |= input_buffer[sample] > ZERO;

  if (data->recording)
  {
    fwrite (input_buffer, sizeof (jack_sample_t), nframes, data->output_file);
    fflush (data->output_file);
  }

  return 0;
}
