#======================================================================
# File:   transition.py
# Author: Benny Saxen
# Date:   2018-01-17
#======================================================================
import sys
#======================================================================
# Configuration
n_devices = 5

#======================================================================
# Global variables
top_value = []
top_index = []

trans = []
index = []
#======================================================================
def bubbleAsc(listA,listB):
    length = len(listA) - 1
    sorted = False

    while not sorted:
        sorted = True
        for i in range(length):
            if listA[i] > listA[i+1]:
                sorted = False
                listA[i], listA[i+1] = listA[i+1], listA[i]
                listB[i], listB[i+1] = listB[i+1], listB[i]
#======================================================================
def bubbleDesc(listA,listB):
    length = len(listA) - 1
    sorted = False

    while not sorted:
        sorted = True
        for i in range(length):
            if listA[i] > listA[i-1]:
                sorted = False
                listA[i], listA[i-1] = listA[i-1], listA[i]
                listB[i], listB[i-1] = listB[i-1], listB[i]
#======================================================================

argc = len(sys.argv)
#print argc

if argc == 1:
    print "no input file given"
    exit()
#======================================================================
# Init
for i in range(0,10):
    top_index.append(1)
    top_index[i] = 0
    top_value.append(1)
    top_value[i] = 0

for i in range(0,100):
    trans.append(1)
    trans[i] = 0
    index.append(1)
    index[i] = 0

imax = 0

filename = sys.argv[1]
print(filename)

file = open(filename, "r")
value = 0.0
#======================================================================
# Read data
for line in file:
    #print line
    line = line.strip('\n')
    temp = line.split(" ")
    hour   = temp[0]
    minute = temp[1]
    second = temp[2]
    old = int(value)
    value  = float(temp[3])
    ix = abs(int(value) - old)
    ix = int(ix/100)
    #print ix
    trans[ix] += 1
    index[ix] = ix
    #updateTopList(imax)
    #print top_index
    #print top_value
    if imax < ix:
        imax = ix
file.close()

file = open("trans.work", "w")
for i in range(0,imax):
    #if trans[i] != 0:
    file.write("%d %d\n" % (i,trans[i]))
        #print "."*freq[i]
        #print i
file.close()


bubbleDesc(trans,index)

def dec_to_bin(x):
    return str(bin(x)[2:])

for i in range(5):
    print ("%d %d" % (i,index[i]))
print "================"
for i in range(32):
    bb = dec_to_bin(i)
    #print bb
    sum = 0
    for j in range(len(bb)):
        #print j
        if bb[j] == '1':
            #print ("sum %d %s" % (j,"ds"))
            sum = sum + int(index[len(bb)-j-1])
    print ("%d %d %s" % (i,sum,bb))
#print trans
#print index
