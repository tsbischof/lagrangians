#!/usr/bin/env python3

import os
import sys
import logging

sys.path.append("/home/tsbischof/lib")

import lagrangians

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)

    if len(sys.argv) == 1:
        print("Usage: lagrangians <input_files>")
    else:
        for filename in sys.argv[1:]:
            lagrangians.Lagrangian(filename).run()
