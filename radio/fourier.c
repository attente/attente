#include <stdio.h>

#include <fftw3.h>

#include "window.h"



int
main (int   argc,
      char *argv[])
{
  if (argc != 1)
  {
    printf ("%s < input > output\n", argv[0]);
    printf ("input [0, %d): bytes per sample\n", (int) sizeof (int));
    printf ("input [%d, %d): number of samples\n", (int) sizeof (int), (int) (2 * sizeof (int)));
    printf ("input [%d, ...): samples\n", (int) (2 * sizeof (int)));
    printf ("output [0, %d): number of complex coefficients\n", (int) sizeof (int));
    printf ("output [%d, ...): complex coefficients\n", (int) sizeof (int));

    return 0;
  }

  int size;
  int length;

  fread (&size,   sizeof (size),   1, stdin);
  fread (&length, sizeof (length), 1, stdin);

  double       *input  = fftw_malloc (length * sizeof (double));
  fftw_complex *output = fftw_malloc ((length / 2 + 1) * sizeof (fftw_complex));

  if (size == 4)
  {
    float sample;

    int i;

    for (i = 0; i < length; i++)
    {
      fread (&sample, size, 1, stdin);

      input[i] = window (length, i) * sample;
    }
  }
  else if (size == 8)
  {
    double sample;

    int i;

    for (i = 0; i < length; i++)
    {
      fread (&sample, size, 1, stdin);

      input[i] = window (length, i) * sample;
    }
  }

  unsigned int flags = FFTW_ESTIMATE | FFTW_DESTROY_INPUT;
  fftw_plan plan = fftw_plan_dft_r2c_1d (length, input, output, flags);

  fftw_execute (plan);

  length = length / 2 + 1;

  fwrite (&length, sizeof (length), 1, stdout);
  fwrite (output, sizeof (fftw_complex), length, stdout);

  fftw_destroy_plan (plan);
  fftw_free (input);
  fftw_free (output);

  return 0;
}
