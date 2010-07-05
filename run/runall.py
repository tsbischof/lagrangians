#!/usr/bin/env python3

from multiprocessing import Pool
import re
import subprocess
import sys
import os
import time

def do_run(filename):
    queue_file = "#!/bin/sh\n"
    lagrangians = "lagrangians"
    base = filename[:-4]
    log_name = base + ".log"

    queue_file += "cd %s\n" % os.getcwd()
    queue_file += "%s %s\n" % (lagrangians, filename)
    queue_file += "convert %s.ppm %s.png\n" % (base, base)
    queue_file += "bzip2 %s.raw %s.ppm\n" % (base, base)

    queue_script = open(base + ".sh", "w")
    queue_script.write(queue_file)
    queue_script.close()

    subprocess.Popen(["qsub", base+".sh"]).wait()

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
