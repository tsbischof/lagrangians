#!/usr/bin/env python3

import sys
import re
import struct
import os
import logging
import collections
import subprocess
import optparse
import logging
import random

sys.path.append("/home/tsbischof/lib")

import lagrangians
from lagrangians import modes

DEFAULT_COLORMAP = [(0, 0, 0),
                    (255, 0, 0),
                    (255, 255, 0),
                    (255, 255, 255)]

def colormaps_from_swatches(
    swatch_dir="/home/tsbischof/src/lagrangians/run/swatches"):
    parser = re.compile("swatch_(?P<points>.+)\.png")
    for root, dirs, files in os.walk(swatch_dir):
        if "like" in root and not "dislike" in root:
            for filename in files:
                parsed = parser.search(filename)
                if parsed:
                    points = list(
                        map(
                            int,
                            parsed.group("points").split("_")))

                    yield([tuple(points[i:i+3]) for i in range(len(points)//3)])
                else:
                    logging.error("Could not parse {0}".format(filename))

def apply_colormap(raw_filename, height, width,
             dst_filename,
             my_colormap=DEFAULT_COLORMAP):
    colormap_cmd = [
        "colormap",
        "--file-in", raw_filename,
        "--height", str(height),
        "--width", str(width),
        "--colormap-length", str(len(my_colormap)),
        "--"]
    colormap_cmd.extend(list(map(str, flatten(my_colormap))))

    convert_cmd = ["convert", "ppm:-", dst_filename]

    colormapper = subprocess.Popen(colormap_cmd, stdout=subprocess.PIPE)
    convert = subprocess.Popen(convert_cmd, stdin=colormapper.stdout).wait()        

def flatten(L):
    for elem in L:
        if isinstance(elem, collections.Iterable) \
           and not isinstance(elem, str):
            for sub in flatten(elem):
                yield(sub)
        else:
            yield(elem)

def downsample(raw_pixels, old_height, old_width, new_height, new_width):
    width_ratio = new_width/float(old_width)
    height_ratio = new_height/float(old_height)

    new_image = [[[0, 0] for i in range(new_width)] for j in range(new_height)]

    for i in range(old_height):
        logging.debug("Sampling row {0} of {1}".format(i, old_height-1))
        for j in range(old_width):
            value = struct.unpack(modes.raw_type,
                                  raw_pixels.read(
                                      struct.calcsize(modes.raw_type)))         
            my_i = int(i*height_ratio)
            my_j = int(j*width_ratio)

            new_image[my_i][my_j][0] += 1
            new_image[my_i][my_j][1] += value[0]

    for i in range(new_height):
        for j in range(new_width):
            new_image[i][j] = new_image[i][j][1]/new_image[i][j][0]

    return(new_image)

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    parser = optparse.OptionParser()
    parser.add_option("-w", "--width", type=int, help="New width",
                        default=None)

    options, args = parser.parse_args()


    colormaps = list(colormaps_from_swatches())

    for i in range(0, 255, 10):
        colormaps.extend([
            [(0, 0, 0),
             (0, 0, i),
             (0, i, i),
             (i, i, i)],
            [(0, 0, 0),
             (i, 0, 0),
             (0, i, 0),
             (0, 0, i)],
            DEFAULT_COLORMAP,
            [(0, 0, 0),
             (i, 0, 0),
             (i, i, 0),
             (i, i, i),
             (0, i, i),
             (0, 0, i),
             (0, 0, 0)],
            [(i, i, i),
             (0, i, i),
             (0, 0, i),
             (0, 0, 0),
             (i, 0, 0),
             (i, i, 0),
             (i, i, i)],
            [(i, i, i),
             (0, i, i),
             (0, 0, i),
             (0, 0, 0),
             (0, 0, i),
             (0, i, i),
             (i, i, i)],
            [(0, 0, 0),
             (0, i, i),
             (i, i, 0),
             (i, 0, 0),
             (0, 0, 0)]])

##    for i in range(100):
##        colormaps.extend([
##			[tuple([random.randint(-255, 255) for i in range(3)])
##             for j in range(5)]])

##    colormaps = [DEFAULT_COLORMAP,
##                 [(255, 255, 255),
##                  (219, 133, 14),
##                  (0, 0, 0),
##                  (133, 185, 214),
##                  (255, 255, 255)],
##                 [(255, 255, 255),
##                  (48, 86, 246),
##                  (0, 0, 0),
##                  (223, 98, 126), 
##                  (0, 0, 0)]]

#    colormaps = list()
#    
#    a = 30
#    for i in range(1000):
#        colormaps.append([(255-random.randint(0, 2*a),
#                           255-random.randint(0, 2*a),
#                           255-random.randint(0, 2*a)),
#                          (255-random.randint(0, 2*a),
#                           59+random.randint(-a, a),
#                           59+random.randint(-a, a)),
#                          (255-random.randint(0, 2*a),
#                           255-random.randint(0, 2*a),
#                           random.randint(0, 2*a)),
#                          (40+random.randint(-a, a),
#                           40+random.randint(-a, a),
#                           150+random.randint(-a, a)),
#                          (random.randint(0, 2*a),
#                           random.randint(0, 2*a),
#                           random.randint(0, 2*a))])
##    colormaps = [DEFAULT_COLORMAP]

    for filename in args:
        image = lagrangians.Lagrangian(filename)

        for my_colormap in colormaps:
            print(filename, my_colormap)
            image.to_png(dst_width=options.width,
                         dst_folder=os.path.join(os.getcwd(),
                                    re.sub("\.inp", "", filename)),
                         my_colormap=my_colormap,
                         compress=False,
                         name_with_colormap=True)

