#!/usr/bin/env python3.1

import os
import subprocess
import sys
import math

def to_RGB(value, limit):
    myval = value % (3*limit-2)
    result = [0, 0, 0]
    for i in range(3):
        if not myval <= 0:
            if myval >= limit:
                result[i] = limit
            else:
                result[i] = myval
            myval -= limit
        else:
            result[i] = 0
    return(result)

def squared(vals, limit):
    return(to_RGB(sum(vals)**2, limit))

def squared_each(vals, limit):
    for i in range(3):
        vals[i] = vals[i]**2 % limit
    return(vals)

def logarithm(vals, limit):
    return(to_RGB(int(abs(limit*math.log(sum(vals))/math.log(limit))), limit))

def kill_mixing(vals, limit):
    for i in range(2):
        if vals[i+1] > 0:
            vals[i] = 0
    return(vals)

def rotate(vals, limit):
    return([vals[1], vals[2], vals[0]])

def new_line(line, func):
    vals = list()
    for val in line.strip().split():
        vals.append(int(val))
        if len(vals) == 3:
            yield(" ".join(map(str, func(vals))))
            vals = list()
        else:
            pass

if __name__ == "__main__":
    for infile_name in sys.argv[1:]:
##    os.chdir("blargh")
##    for infile_name in ["dangling_stick00000009.ppm"]:
        outfile_name = infile_name[:-4] + "_new.ppm"

        infile = open(infile_name, "r")
        outfile = open(outfile_name, "w")

        in_data = False
        next_line = False # if true, next line will have the limit
        
        for line in infile:               
            if not in_data:
                if next_line:
                    limit = int(line)
                    next_line = False
                if len(line.split()) == 2:
                    next_line = True
                if len(line.split()) > 3:
                    in_data = True

            if in_data:
                for newval in new_line(line, \
                         lambda x: squared_each(x, limit)):
                    outfile.write(newval + "  ")
                outfile.write("\n")
            else:
                outfile.write(line)

        infile.close()
        outfile.close()

        subprocess.Popen(["convert", outfile_name,
                          outfile_name[:-4] + ".png"]).wait()
