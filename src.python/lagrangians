#!/usr/bin/env python3

import os
import sys

import grapher

def main(argv):
    for filename in sys.argv[1:]:
        my_grapher = grapher.Grapher(filename)
        my_grapher.do_image()
        my_grapher.to_ppm()
        my_grapher.to_png()

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Usage: lagrangians <input_files>")
    else:
        main(sys.argv)
