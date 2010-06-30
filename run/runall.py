#!/usr/bin/env python3

from multiprocessing import Pool
import re
import subprocess
import sys
import os
import time

def do_run(filename):
    lagrangians = "/home/tsbischof/src/lagrangians/lagrangians"
    base = filename[:-3]
    log_name = base + "log"
    log_file = open("log", "a")
    log_file.write("%s: Working with input file %s.\n" % (time.strftime("%Y.%m.%d %X"), filename))
    log_file.close()
    log_file = open(log_name, "w")
    subprocess.Popen([lagrangians, filename], stdout=log_file, stderr=log_file).wait()
    subprocess.Popen(["convert", base + "ppm", base + "png"]).wait()
    subprocess.Popen(["bzip2", base + "raw"]).wait()
    subprocess.Popen(["convert", base + "ppm", base + "png"]).wait()
    subprocess.Popen(["bzip2", base + "ppm"]).wait()
    log_file.close()

if __name__ == "__main__":
    os.nice(19)
    pool = Pool(1)

    input_files = list()
    for filename in filter(lambda x: x.endswith(".inp"),
         map(lambda y: y.strip(), sys.stdin.readlines())):
        if os.path.isfile(filename) \
           and not os.path.isfile(re.sub("inp$", "png", filename)):
            input_files.append(filename)

    pool.map(do_run, input_files)
