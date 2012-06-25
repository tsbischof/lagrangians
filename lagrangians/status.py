#!/usr/bin/env python3

import logging
import os
import sys

sys.path.append("/home/tsbischof/lib")
import lagrangians

def find_files():
    for root, dirs, files in os.walk(os.getcwd()):
        for filename in filter(lambda x: x.endswith(".inp"), files):
            myfile = os.path.join(root, filename)
            base = myfile[:-4]
            if os.path.isfile(base+".restart"):
                yield(myfile)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    filenames = sys.argv[1:]
    if len(filenames) == 0:
        filenames = find_files()
        
    for filename in sorted(filenames):
        lagrangians.Lagrangian(filename).status()
        
