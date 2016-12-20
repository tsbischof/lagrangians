import sys
import math

def choose_rgb(themax, val):
    result = str()
    limit = 255
    
    myval = int(math.ceil((limit*3+2)*math.sqrt(val/themax)))

    for i in range(3):
        if myval < 0:
            result += "0"
        elif myval > limit:
            result += "%d" % limit
        else:
            result += "%d" % myval

        myval -= (limit+1)
        result += " "

    return(result)

image = list()

filename = sys.argv[1]
f = open(filename, "r")
i = 0

themax = 0

for line in f:
    if i >= 1:
        mymax = max(map(float, line.strip().split(" ")))
        if mymax > themax:
            themax = mymax

    i += 1

f.close()

base = filename[:-4]
i = 0
infile = open(filename, "r")
outfile = open(base + ".ppm", "w")

for line in infile:
    if i >= 1:
        outfile.write(" ".join(list(map(lambda x: choose_rgb(themax, float(x)),\
                                        line.strip().split(" ")))))
    else:
        outfile.write("P3\n")
        outfile.write(line)
        outfile.write("255\n")

    i += 1

infile.close()
outfile.close()
