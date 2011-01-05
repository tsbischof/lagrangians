#!/usr/bin/env python3

import subprocess
import itertools
import os
import struct
import sys

sys.path.append("/home/tsbischof/src/lagrangians/run")

import colormap

def make_swatch(filename, resolution, height, width):
    f = open(filename, "wb")
    for i in range(height):
        for j in range(width):
            f.write(struct.pack("d", j+1))
    f.close()

def flatten(L):
    result = list()
    for elem in L:
        result.extend(elem)
    return(result)
   
def make_swatches(n, n_swatches=0, in_colormap=None):
    swatches = "swatches_%d" % n
    try:
        os.mkdir(swatches)
    except:
        pass
    os.chdir(swatches)

    source = "template.raw"
    resolution = 1
    out_base = "swatch_"
    height = 50
    width = 1000
    depth = 255

    make_swatch(source, resolution, height, width)

    for i in range(n_swatches):
        mycolormap = colormap.random_colormap(n, depth)
        out_name = out_base + "_".join(map(str, mycolormap)) + ".png"
        print(out_name)
        colormap.do_colormap(source, out_name, resolution, \
                             height, width, n, mycolormap)

    if in_colormap:
        mycolormap = in_colormap
        out_name = out_base + "_".join(map(str, mycolormap)) + ".png"
        print(out_name)
        colormap.do_colormap(source, out_name, resolution, \
                             height, width, n, mycolormap)

    
    p1 = [0,0,0]
    p4 = [255,255,255]
##    for p2 in itertools.permutations([0,0,0,255,255,255],3):
##        for p3 in itertools.permutations([0,0,0,255,255,255],3):
##            mycolormap = flatten([p1, list(p2), list(p3), p4])
    for p2 in itertools.permutations([0,0,255]):
        for p3 in itertools.permutations([0,255,255],3):
            for perm in itertools.permutations([p1, list(p2), list(p3), p4]):
                mycolormap = flatten(perm)
                out_name = out_base + "_".join(map(str, mycolormap)) + ".png"
                if not os.path.isfile(out_name):
                    print(out_name)
                    colormap.do_colormap(source, out_name, resolution, \
                                         height, width, n, mycolormap)
    os.chdir("..")

if __name__ == "__main__":
    n_swatches = 0
##    for n in range(5,6):
    make_swatches(4, n_swatches)
