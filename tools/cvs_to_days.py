import csv

with open('data.csv', 'rb') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='\"')
    for row in reader:
        #print row[1] # electric power
        #print row[4] # date and time
        ds = row[4].split(" ")
        xdate = ds[0]
        xtime = ds[1].replace(':', ' ')
        filename = 'ioant_' + xdate + ".data"
        print filename
        with open(filename, 'a') as file:
            file.write('%s %s\n' % (xtime,row[1]))
        file.close()
        #for col in row:
        #    print col
csvfile.close()
