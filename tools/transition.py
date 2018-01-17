#======================================================================
# File:   transition.py
# Author: Benny Saxen
# Date:   2018-01-17
#======================================================================
import sys
#======================================================================
# Configuration
n_devices = 4
MAXI = 9000
#======================================================================
# Global variables
trans = []
index = []
sumint = []
mx = []
#======================================================================
def dec_to_bin(x):
    return str(bin(x)[2:])
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

for i in range(0,MAXI):
    trans.append(1)
    index.append(1)
    sumint.append(1)
    mx.append(1)

for n_devices in range(1,12):
#======================================================================
# Init
    for i in range(0,MAXI):
        trans[i] = 0
        index[i] = 0
        sumint[i] = 0
        mx[i] = 0

    imax = 0

    filename = sys.argv[1]
    #print(filename)
        #======================================================================
    file = open(filename, "r")
    value = 0.0
    #======================================================================
    # Read data
    for line in file:
        #  print line
        line = line.strip('\n')
        temp = line.split(" ")
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
#======================================================================
    file = open("trans.work", "w")
    for i in range(0,imax):
    #if trans[i] != 0:
        file.write("%d %d\n" % (i,trans[i]))
        #print "."*freq[i]
        #print i
    file.close()

#======================================================================
    bubbleDesc(trans,index)

    for k in range(n_devices):
        print ("%d %d" % (k,index[k]))
    print "================"
    for i in range(2**n_devices):
        bb = dec_to_bin(i)
        sumint[i] = 0
        for j in range(len(bb)):
            if bb[j] == '1':
                sumint[i] = sumint[i] + int(index[len(bb)-j-1])
        mx[sumint[i]] = 1

#======================================================================
    file = open("freq.work", "r")
#======================================================================
# Read data
    tot = 0
    i = 0
    hit = 0
    for line in file:
        #print line
        line = line.strip('\n')
        temp = line.split(" ")
        power = int(temp[0])
        freq  = int(temp[1])
        hit = hit + mx[power]*freq
        tot = tot + freq
    file.close()

    #print ("%d tot=%d hit=%d" % (n_devices,tot,hit))
    ftemp =  float(hit)/float(tot)
    print ("%d %f" % (n_devices,ftemp))
