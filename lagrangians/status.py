#!/usr/bin/env python3

import os
import sys

import lagrangians

def find_files():
    for root, dirs, files in os.walk(os.getcwd()):
        for filename in filter(lambda x: x.endswith(".inp"), files):
            myfile = os.path.join(root, filename)
            base = myfile[:-4]
            if os.path.isfile(base+".restart"):
                yield(myfile)

if __name__ == "__main__":
    filenames = sys.argv[1:]
    if len(filenames) == 0:
        filenames = find_files()
        
    for filename in sorted(filenames):
        lagrangians.Lagrangian(filename).status()
        
