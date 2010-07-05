#!/usr/bin/env python3

import os
import sys
import configparser
import subprocess
import tempfile
import math
import struct

def get_height(filename):
    config_file = tempfile.TemporaryFile("w+t")
    config_file.write('[config]\n')
    f = open(filename, "r")
    for line in f:
        config_file.write(line)
    f.close()
    config_file.seek(0)

    parser = configparser.SafeConfigParser()
    parser.readfp(config_file)
    y = parser.get("config", "plot").split(",")[0]
    y_limits = list(map(float, parser.get("config", y).split(",")))
    height = math.ceil((y_limits[2]-y_limits[0])/y_limits[1]+1)
    config_file.close()
    return(height)

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

if __name__ == "__main__":
    for filename in sys.argv[1:]:
        height = get_height(filename)
        completed = read_restart(filename, height)
        if completed >= 0:
            print("%s: Finished %d of %d rows." % (filename, completed, height))
##    parse config, determine height
##    subprocess with `od -N 4*height --width=4 -vi blargh.inp | grep " 0" | wc -l` to get line count
##    print("%s: finished %d lines of %d." % (completed, height))
