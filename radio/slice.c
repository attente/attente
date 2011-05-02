#include <stdlib.h>
#include <stdio.h>



int
main (int   argc,
      char *argv[])
{
  if (argc != 3)
  {
    printf ("%s offset length < input > output\n", argv[0]);
    printf ("input [0, %d): bytes per sample\n", (int) sizeof (int));
    printf ("input [%d, ...): samples\n", (int) sizeof (int));
    printf ("output [0, %d): bytes per sample\n", (int) sizeof (int));
    printf ("output [%d, %d): number of samples\n", (int) sizeof (int), (int) (2 * sizeof (int)));
    printf ("output [%d, ...): samples\n", (int) (2 * sizeof (int)));

    return 0;
  }

  int offset = atoi (argv[1]);
  int length = atoi (argv[2]);

  int size;

  fread  (&size,   sizeof (size),   1, stdin);
  fwrite (&size,   sizeof (size),   1, stdout);
  fwrite (&length, sizeof (length), 1, stdout);

  float  zerof = 0;
  float  copyf = 0;
  double zerod = 0;
  double copyd = 0;

  void *zero = NULL;
  void *copy = NULL;

  if (size == 4)
  {
    zero = &zerof;
    copy = &copyf;
  }
  else if (size == 8)
  {
    zero = &zerod;
    copy = &copyd;
  }

  for (; offset < 0 && length > 0; offset++, length--)
    fwrite (zero, size, 1, stdout);

  if (length < 0)
    return 0;

  for (; offset > 0; offset--)
    fread (copy, size, 1, stdin);

  for (; length > 0 && fread (copy, size, 1, stdin); length--)
    fwrite (copy, size, 1, stdout);

  for (; length > 0; length--)
    fwrite (zero, size, 1, stdout);

  return 0;
}
