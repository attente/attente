#!/usr/bin/env python



# Two former high-school friends, Bob and Joe, accidentally meet after
# having not seen each other for years and years.
#
# Bob: "Joe, are you married yet?"
#
# Joe: "Yes, my wife and I have three healthy and happy children"
#
# Bob: "How old are they?"
#
# Joe: "Bob, I remember you enjoyed math puzzles in school. If you
# multiply my three childrens' ages together, the product is 36"
#
# Bob: "I still do like math puzzles, but you're going to have to give
# me more information for me to solve the ages of your children"
#
# Joe: "Ok, if you add my three childrens' ages together, the sum is the
# same as the number on that bus that's passing by right now"
#
# Bob:" Ok, I see the number on that bus, but I still need a little more
# information before I can determine the ages of your children"
#
# Joe: "Ok, my older child's eyes are blue"
#
# Bob: "Thank you Joe. I now know that your childrens' ages are ____,
# _____ and ____"
#
# What are Joe's childrens' ages?



import constraint



def composite (n):
  subproblem1 = constraint.Problem ()
  subproblem2 = constraint.Problem ()

  # [0, 1, 2]
  e = range (3)

  # three ages that divide into 36
  subproblem1.addVariables (e, [2 ** i * 3 ** j for i in e for j in e])
  subproblem2.addVariables (e, [2 ** i * 3 ** j for i in e for j in e])

  # the product of their ages is 36
  subproblem1.addConstraint (lambda x, y, z: x * y * z == 36)
  subproblem2.addConstraint (lambda x, y, z: x * y * z == 36)

  # the sum of their ages is n
  subproblem1.addConstraint (lambda x, y, z: x + y + z == n)
  subproblem2.addConstraint (lambda x, y, z: x + y + z == n)

  # two cases: unique eldest and non-unique
  subproblem1.addConstraint (lambda x, y, z: x <= y and y < z)
  subproblem2.addConstraint (lambda x, y, z: x <= y and y == z)

  # count how many solutions there are in each case
  return len (subproblem1.getSolutions () or []) == 1 and len (subproblem2.getSolutions () or []) > 1



def main (argv):
  problem = constraint.Problem ()

  # [0, 1, 2]
  e = range (3)

  # three ages that divide into 36
  problem.addVariables (e, [2 ** i * 3 ** j for i in e for j in e])

  # the product of their ages is 36
  problem.addConstraint (lambda x, y, z: x * y * z == 36)

  # the sum has multiple decompositions with product 36
  problem.addConstraint (lambda x, y, z: composite (x + y + z))

  # there is only one eldest child
  problem.addConstraint (lambda x, y, z: x <= y and y < z)

  # print all satisfying solutions
  print (problem.getSolutions ())



if __name__ == '__main__':
  from sys import argv
  main (argv)
