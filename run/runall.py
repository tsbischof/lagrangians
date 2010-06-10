#!/usr/bin/env python

from multiprocessing import Pool
import re
import subprocess
import sys
import os

def do_run(filename):
    lagrangians = "/home/tsbischof/src/lagrangians/src/lagrangians"
    log_name = re.sub("inp$", "log", filename)
    print("Working with input file", filename)
    log_file = open(log_name, "w")
    subprocess.Popen([lagrangians, filename], stdout=log_file).wait()
    log_file.close()

if __name__ == "__main__":
    pool = Pool()

    input_files = list()
    for filename in filter(lambda x: x.endswith(".inp"), sys.argv):
        if os.path.isfile(filename) \
           and not os.path.isfile(re.sub("inp$", "raw", filename)):
            input_files.append(filename)

    pool.map(do_run, input_files)
