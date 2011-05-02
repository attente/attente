#!/usr/bin/env python



import sys
import math
import struct
import matplotlib.pyplot



def main(argv):
  n = struct.unpack_from('i', sys.stdin.read(struct.calcsize('i')))[0]
  x = [struct.unpack_from('dd', sys.stdin.read(struct.calcsize('dd'))) for _ in xrange(n)]
  y = [math.sqrt(z[0] * z[0] + z[1] * z[1]) for z in x]

  matplotlib.pyplot.plot(y)
  matplotlib.pyplot.show()



if __name__ == '__main__':
  main(sys.argv[1:])
