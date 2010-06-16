#!/usr/bin/env python

from multiprocessing import Pool
import re
import subprocess
import sys
import os

def do_run(filename):
    lagrangians = "/home/tsbischof/src/lagrangians/lagrangians"
    base = filename[:-3]
    if os.path.isfile(base+"png"):
        return()
    log_name = base + "log"
    print("Working with input file", filename)
    log_file = open(log_name, "w")
    subprocess.Popen([lagrangians, filename], stdout=log_file).wait()
    subprocess.Popen(["convert", base + "ppm", base + "png"]).wait()
    subprocess.Popen(["bzip2", base + "raw"]).wait()
    subprocess.Popen(["convert", base + "ppm", base + "png"]).wait()
    subprocess.Popen(["bzip2", base + "ppm"]).wait()
    log_file.close()

if __name__ == "__main__":
    pool = Pool(1)

    input_files = list()
    for filename in filter(lambda x: x.endswith(".inp"), sys.argv):
        if os.path.isfile(filename) \
           and not os.path.isfile(re.sub("inp$", "raw", filename)):
            input_files.append(filename)

    pool.map(do_run, input_files)
