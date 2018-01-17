import sys

freq = []

argc = len(sys.argv)
#print argc

if argc == 1:
    print "no input file given"
    exit()

for i in range(0,10000):
    freq.append(1)
    freq[i] = 0

imax = 0

filename = sys.argv[1]
print(filename)

file = open(filename, "r")
for line in file:
    #print line
    line = line.strip('\n')
    temp = line.split(" ")
    hour   = temp[0]
    minute = temp[1]
    second = temp[2]
    value  = float(temp[3])
    ix = int(value)
    ix = int(ix/100)
    #print ix
    freq[ix] += 1
    if imax < ix:
        imax = ix
file.close()

file = open("freq.work", "w")
for i in range(0,imax):
    if freq[i] != 0:
        file.write("%d %d\n" % (i,freq[i]))
        #print "."*freq[i]
        #print i
file.close()
