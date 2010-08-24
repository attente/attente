#!/usr/bin/env python

#        sea
#    -------
# ale|raison
#     rros
#     ----
#      erio
#      ponl
#      ----
#       npnn
#       nmrr
#       ----
#        mee



from constraint import *



problem = Problem ()

for x in ['r', 'a', 'i', 's', 'n', 'l', 'e', 'p', 'm', 'o']:
  problem.addVariable (x, range (10))

problem.addConstraint (lambda i: i == 0, ('i',))
problem.addConstraint (lambda o, r: o + r == 10, ('o', 'r'))
problem.addConstraint (lambda a, r, e: a % 10 == r + e + 1, ('a', 'r', 'e'))
problem.addConstraint (lambda e, s: e * s % 10 == s, ('e', 's'))
problem.addConstraint (lambda e, l: e * e % 10 == l, ('e', 'l'))
problem.addConstraint (lambda e, a, r: e * a % 10 == r, ('e', 'a', 'r'))
problem.addConstraint (lambda n, r, e: (r + e) % 10 == n, ('n', 'r', 'e'))
problem.addConstraint (lambda m, p: (m + m) % 10 == p, ('m', 'p'))
problem.addConstraint (AllDifferentConstraint ())

solution = problem.getSolution ()
solution = solution.items ()
solution.sort (None, lambda x: x[1])
solution = ''.join ([x[0] for x in solution])

print solution
