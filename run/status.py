#!/usr/bin/env python3

import os
import sys
import configparser
import subprocess
import tempfile
import math
import struct

def read_config(filename):
    config_file = tempfile.TemporaryFile("w+t")
    config_file.write('[config]\n')
    f = open(filename, "r")
    for line in f:
        config_file.write(line)
    f.close()
    config_file.seek(0)

    parser = configparser.SafeConfigParser()
    parser.readfp(config_file)
    return(parser)

def get_t_limits(parser):
    return(list(map(float, parser.get("config", "t").split(","))))

def get_size(parser):
    plot = map(lambda x: x.strip(), parser.get("config", "plot").split(","))
    limits = map(lambda x: list(map(float, parser.get("config", x).split(","))), plot)
    size = list(map(lambda x: int(x[1]), limits))
    return(size)

def read_restart(filename, height):
    restart_filename = filename[:-4] + ".restart"
    try:
        restart_file = open(restart_filename, "rb")

        completed = 0
        for i in range(height):
            byte = restart_file.read(4)
            val = struct.unpack("i", byte)[0]
            if val == 1:
                completed += 1
            
        restart_file.close()
        return(completed)
    except:
        print("Could not read restart file %s." % restart_filename)
        return(-1)

def find_files():
    for root, dirs, files in os.walk(os.getcwd()):
        for filename in files:
            myfile = os.path.join(root, filename)
            if myfile.endswith(".inp"):
                base = myfile[:-4]
                if os.path.isfile(base+".restart"):
                    yield(myfile)
#                    if os.path.isfile(base+".png"):
#                        print("%s is finished." % myfile)
#                    else:
#                        filenames.append(myfile)

if __name__ == "__main__":
    filenames = sys.argv[1:]
    if len(filenames) == 0:
        filenames = find_files()
        
    for filename in sorted(filenames):
        parser = read_config(filename)
        height, width = get_size(parser)
        completed = read_restart(filename, height)
        if completed >= 0:
            print("%s: Finished %d of %d rows." % (filename, completed, height))
##    parse config, determine height
##    subprocess with `od -N 4*height --width=4 -vi blargh.inp | grep " 0" | wc -l` to get line count
##    print("%s: finished %d lines of %d." % (completed, height))
