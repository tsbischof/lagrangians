#!/usr/bin/env python3

import ctypes
import math
import os
import sys
import random
import shutil

import utils
import grapher
from c_libraries import colormapper

def colormap_to_array(colormap):
    n_points = len(colormap)

    my_colormap = (ctypes.POINTER(ctypes.c_int)*n_points)()
    for i in range(n_points):
        my_colormap[i] = (ctypes.c_int*3)()
        for j in range(3):
            my_colormap[i][j] = colormap[i][j]

    return((ctypes.c_int(n_points), my_colormap))

def do_colormap(raw_filename, ppm_filename, height, width, \
                time_resolution, colormap):
    n_points, my_colormap = colormap_to_array(colormap)

    colormapper.raw_to_ppm( \
            ctypes.create_string_buffer(raw_filename.encode()), \
            ctypes.create_string_buffer(ppm_filename.encode()), \
            ctypes.c_double(time_resolution), \
            ctypes.c_double(0),\
            ctypes.c_int(height), ctypes.c_int(width),\
            n_points, my_colormap)

def do_phase_to_image(raw_filename, ppm_filename, height, width, colormap):
    n_points, my_colormap = colormap_to_array(colormap)
    colormapper.raw_to_ppm( \
            ctypes.create_string_buffer(raw_filename.encode()), \
            ctypes.create_string_buffer(ppm_filename.encode()), \
            ctypes.c_double(0.001), \
            ctypes.c_double(2*math.pi),\
            ctypes.c_int(height), ctypes.c_int(width), 
            n_points, my_colormap)

# What follows are tools which help us deal with color swatches and applying
# new colormaps to existing files.
def random_colormap(n, depth):
    return(list_to_colormap(\
        [random.randint(-depth, depth) for i in range(3*n)]))

def colormap_to_str(colormap):
    result = list()
    for point in colormap:
        result.extend(point)
    return("_".join(map(str, result)))

def list_to_colormap(L):
    """Take a list of values and return a list of RGB triples."""
    result = list()
    for index in range(len(L)//3):
        result.append(L[3*index:3*index+3])
    return(result)
        
def get_swatches(swatch_dir="/home/tsbischof/src/lagrangians/run/swatches"):
    colormaps = list()
    for root, dirs, files in os.walk(swatch_dir):
        if "/like" in root:
            for file in filter(lambda x: x.endswith(".png"), files):
                colormap = file.split("_")[1:]
                colormap[-1] = colormap[-1].split(".")[0]
                colormap = list(map(int, colormap))
                colormaps.append(list_to_colormap(colormap))
    return(colormaps)

def do_variety(files=None, depth=255, colormaps=None, n=4, overwrite=False):
    if not colormaps:
        # Did not specify colormaps, get some number of them and use those.
        colormaps = iter(random_colormap(n, depth) for i in range(50))

    for filename in files:
        my_grapher = grapher.Grapher(filename)

        if os.path.isfile(my_grapher.filename("raw.bz2")):
            utils.decompress(my_grapher.filename("raw.bz2"))

        folder = my_grapher.filename()
        
        try:
            # Create a folder for each input file
            os.mkdir(folder)
        except: 
            pass

        finished = list()
        if not overwrite:
            # Get all of the old images, so we do not repeat them
            for root, dirs, my_files in os.walk(folder):
                finished.extend(my_files)

        for colormap in colormaps:
            n = len(colormap)
            out_base = os.path.join(folder, \
                    "%s_%s" % (folder, colormap_to_str(colormap)))
            if not os.path.split(out_base+".png")[1] in finished:
                print(out_base)
                my_grapher.to_ppm(colormap, my_grapher.filename("raw"),\
                                  out_base+".ppm")
                my_grapher.to_png(colormap, out_base+".ppm", out_base+".png")
                os.remove(out_base+".ppm")
        

if __name__ == "__main__":
    if len(sys.argv) > 1:
        colormaps = [[[255,255,255], [0,0,0], [255,0,0],[255,255,0]], \
                     [[0,0,0], [255,0,0], [255,255,0], [255,255,255]]]
        do_variety(files=sys.argv[1:], colormaps=colormaps, overwrite=False)
    else:
        my_grapher = grapher.Grapher("test.inp")
        my_grapher.do_run()
