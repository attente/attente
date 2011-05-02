#include <unistd.h>

#include <fftw3.h>
#include <jack/jack.h>



static const char * const CLIENT_NAME = "radio";
static const char * const INPUT_NAME  = "input";
static const char * const OUTPUT_NAME = "output";



typedef jack_default_audio_sample_t jack_sample_t;



typedef struct
{
  jack_port_t *input_port;
  jack_port_t *output_port;
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

  jack_set_process_callback (client, process, &data);

  jack_activate (client);

  while (pause ());

  jack_port_unregister (client, data.input_port);
  jack_port_unregister (client, data.output_port);

  jack_deactivate   (client);
  jack_client_close (client);

  return 0;
}



int
process (jack_nframes_t  nframes,
         void           *arg)
{
  state *data = arg;

  jack_sample_t *input_buffer  = jack_port_get_buffer (data->input_port,  nframes);
  jack_sample_t *output_buffer = jack_port_get_buffer (data->output_port, nframes);

  return 0;
}
