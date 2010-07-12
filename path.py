#!/usr/bin/env python



from itertools import permutations



def search (p):
  n = set (sum (p, []))
  k = set (p[0]) & set (p[-1])
  x = set (p[0]) - k
  z = set (p[-1]) - k
  y = n - k - x - z

  n = list (n)
  k = list (k)
  x = list (x)
  z = list (z)
  y = list (y)

  s = []
  m = dict ((e, e) for e in n)
  l = {}

  m.update ((x[i], z[i]) for i in range (len (x)))
  m.update ((z[i], x[i]) for i in range (len (x)))

  for a in permutations (x):
    l.update ((x[i], a[i]) for i in range (len (x)))

    for b in permutations (y):
      l.update ((y[i], b[i]) for i in range (len (y)))

      for c in permutations (z):
        l.update ((z[i], c[i]) for i in range (len (z)))

        for d in permutations (k):
          l.update ((k[i], d[i]) for i in range (len (k)))

          s.append ([[l[v] for v in f] for f in p])
          s.append ([[l[m[v]] for v in f] for f in p])

  r = []

  for q in s:
    if q not in r:
      r.append (q)

  return r



def main (argv):
  for p in search (input ('path = ')):
    print p



if __name__ == '__main__':
  from sys import argv
  main (argv)
