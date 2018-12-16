#!/usr/bin/env python

import random # for seed, random
import sys    # for stdout



################################### TEST PART ##################################
################################################################################

# Tests align strands and scores
# Parameters types:
#    score          =  int   example: -6
#    plusScores     = string example: "  1   1  1"
#    minusScores    = string example: "22 111 11 "
#    strandAligned1 = string example: "  CAAGTCGC"
#    strandAligned2 = string example: "ATCCCATTAC"
#
#   Note: all strings must have same length
def test(score, plusScores, minusScores, strandAligned1, strandAligned2):
    print("\n>>>>>>START TEST<<<<<<")

    if testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
        sys.stdout.write(">>>>>>>Test SUCCESS:")
        sys.stdout.write("\n\t\t" + "Score: "+str(score))
        sys.stdout.write("\n\t\t+ " + plusScores)
        sys.stdout.write("\n\t\t  " + strandAligned1)
        sys.stdout.write("\n\t\t  " + strandAligned2)
        sys.stdout.write("\n\t\t- " + minusScores)
        sys.stdout.write("\n\n")
    else:
        sys.stdout.write("\t>>>>!!!Test FAILED\n\n")


# converts character score to int
def testScoreToInt(score):
    if score == ' ':
        return 0
    return int(score)


# computes sum of scores
def testSumScore(scores):
    result = 0
    for ch in scores:
        result += testScoreToInt(ch)
    return result


# test each characters and scores
def testValidateEach(ch1, ch2, plusScore, minusScore):
    if ch1 == ' ' or ch2 == ' ':
        return plusScore == 0 and minusScore == 2
    if ch1 == ch2:
        return plusScore == 1 and minusScore == 0
    return plusScore == 0 and minusScore == 1


# test and validates strands
def testStrands(score, plusScores, minusScores, strandAligned1, strandAligned2):
    if len(plusScores) != len(minusScores) or len(minusScores) != len(strandAligned1) or len(strandAligned1) != len(
            strandAligned2):
        sys.stdout.write("Length mismatch! \n")
        return False

    if len(plusScores) == 0:
        sys.stdout.write("Length is Zero! \n")
        return False

    if testSumScore(plusScores) - testSumScore(minusScores) != score:
        sys.stdout.write("Score mismatch to score strings! TEST FAILED!\n")
        return False
    for i in range(len(plusScores)):
        if not testValidateEach(strandAligned1[i], strandAligned2[i], testScoreToInt(plusScores[i]),
                                testScoreToInt(minusScores[i])):
            sys.stdout.write("Invalid scores for position " + str(i) + ":\n")
            sys.stdout.write("\t char1: " + strandAligned1[i] + " char2: " +
                             strandAligned2[i] + " +" + str(testScoreToInt(plusScores[i])) + " -" +
                             str(testScoreToInt(minusScores[i])) + "\n")
            return False

    return True

######################## END OF TEST PART ######################################
################################################################################


# Computes the score of the optimal alignment of two DNA strands.
def findOptimalAlignment(strand1, strand2 , map):
	tmp = strand1 + "." + strand2
	if tmp in map: return map[tmp][0]
	tmp = strand2 + "." + strand1
	if tmp in map: return map[tmp][0]
	# if one of the two strands is empty, then there is only
	# one possible alignment, and of course it's optimal
	if len(strand1) == 0:
		plus =""
		minus =""
		aligned1 =""
		for x in range(len(strand2)):
			plus += " "
			minus += "2"
			aligned1 += " "
		tmp = strand1 + "." + strand2
		value = [len(strand2) * -2 , aligned1 , strand2 , plus , minus]
		map[tmp] = value
		return len(strand2) * -2
	if len(strand2) == 0:
		plus =""
		minus =""
		aligned2 =""
		for x in range(len(strand1)):
			plus += " "
			minus += "2"
			aligned2 += " "
		tmp = strand1 + "." + strand2
		value = [len(strand1) * -2 , strand1 , aligned2 , plus , minus]
		map[tmp] = value
		return len(strand1) * -2

	aligned1 = ""
	aligned2 = ""
	plus = ""
	minus = ""
	value = [0 , aligned1 , aligned2 , plus, minus]
	key = strand1 + "."+ strand2

	# There's the scenario where the two leading bases of
	# each strand are forced to align, regardless of whether or not
	# they actually match.
	bestWith = findOptimalAlignment(strand1[1:], strand2[1:] , map)
	tmp = strand1[1:] + "." + strand2[1:]
	if not tmp in map:
		tmp = strand2[1:] + "." + strand1[1:]
	if strand1[0] == strand2[0]:
		aligned1 = strand1[0] + map[tmp][1]
		aligned2 = strand2[0] + map[tmp][2]
		plus = "1" + map[tmp][3]
		minus = " " + map[tmp][4]
		value = [bestWith + 1 , aligned1 , aligned2 , plus, minus]
		map[key] = value
		return bestWith + 1  # no benefit from making other recursive calls

	aligned1 = strand1[0] + map[tmp][1]
	aligned2 = strand2[0] + map[tmp][2]
	plus = " " + map[tmp][3]
	minus = "1" + map[tmp][4]
	value = [bestWith - 1 , aligned1 , aligned2 , plus, minus]
	best = bestWith - 1
	
	# It's possible that the leading base of strand1 best
	# matches not the leading base of strand2, but the one after it.
	bestWithout = findOptimalAlignment(strand1, strand2[1:] , map)
	bestWithout -= 2 # penalize for insertion of space
	if bestWithout > best:
		tmp = strand1 + "." + strand2[1:]
		if not tmp in map:
			tmp = strand2[1:] + "." + strand1
		aligned1 = " " + map[tmp][1]
		aligned2 = strand2[0] + map[tmp][2]
		plus = " " + map[tmp][3]
		minus = "2" + map[tmp][4]
		value = [bestWithout , aligned1 , aligned2 , plus, minus]
		best = bestWithout

	# opposite scenario
	bestWithout = findOptimalAlignment(strand1[1:], strand2 , map)
	bestWithout -= 2 # penalize for insertion of space	
	if bestWithout > best:
		tmp = strand1[1:] + "." + strand2
		if not tmp in map:
			tmp = strand2 + "." + strand1[1:]
		aligned1 = strand1[0] + map[tmp][1]
		aligned2 = " " + map[tmp][2]
		plus = " " + map[tmp][3]
		minus = "2" + map[tmp][4]
		value = [bestWithout , aligned1 , aligned2 , plus, minus]
		best = bestWithout
	map[key] = value
	return best

# Utility function that generates a random DNA string of
# a random length drawn from the range [minlength, maxlength]
def generateRandomDNAStrand(minlength, maxlength):
	assert minlength > 0, \
	       "Minimum length passed to generateRandomDNAStrand" \
	       "must be a positive number" # these \'s allow mult-line statements
	assert maxlength >= minlength, \
	       "Maximum length passed to generateRandomDNAStrand must be at " \
	       "as large as the specified minimum length"
	strand = ""
	length = random.choice(xrange(minlength, maxlength + 1))
	bases = ['A', 'T', 'G', 'C']
	for i in xrange(0, length):
		strand += random.choice(bases)
	return strand

# Method that just prints out the supplied alignment score.
# This is more of a placeholder for what will ultimately
# print out not only the score but the alignment as well.

def printAlignment(score, out = sys.stdout):	
	out.write("Optimal alignment score is " + str(score) + "\n")

# Unit test main in place to do little more than
# exercise the above algorithm.  As written, it
# generates two fairly short DNA strands and
# determines the optimal alignment score.
#
# As you change the implementation of findOptimalAlignment
# to use memoization, you should change the 8s to 40s and
# the 10s to 60s and still see everything execute very
# quickly.
 
def main():
	map = {}
	while (True):
		sys.stdout.write("Generate random DNA strands? ")
		answer = sys.stdin.readline()
		if answer == "no\n": break
		strand1 = generateRandomDNAStrand(200, 201)
		strand2 = generateRandomDNAStrand(200, 201)
		sys.stdout.write("Aligning these two strands: " + strand1 + "\n")
		sys.stdout.write("                            " + strand2 + "\n")
		alignment = findOptimalAlignment(strand1, strand2 , map)
		printAlignment(alignment)
		tmp = strand1 + "." + strand2
		if not tmp in map:
			tmp = strand2 + "." + strand1
		test(alignment, map[tmp][3], map[tmp][4], map[tmp][1], map[tmp][2])
	
if __name__ == "__main__":
  main()
