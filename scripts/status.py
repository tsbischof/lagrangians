#!/usr/bin/env python3

import logging
import os
import sys
import re

import lagrangians

def find_files():
    for root, dirs, files in os.walk(os.getcwd()):
        for filename in filter(lambda x: x.endswith(".restart"), files):
            restart_filename = os.path.join(root, filename)
            inp_filename = re.sub("restart$", "inp", restart_filename)
            if os.path.isfile(inp_filename):
                yield(inp_filename)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    filenames = sys.argv[1:]
    if len(filenames) == 0:
        filenames = find_files()
        
    for filename in filenames:
        lagrangians.Lagrangian(filename).status()
        
