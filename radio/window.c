#include <math.h>



double hann (int N,
             int n);



double (*window) (int N, int n) = hann;



double
hann (int N,
      int n)
{
  return 0.5 * (1 - cos (2 * M_PI * n / (N - 1)));
}
