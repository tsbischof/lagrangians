#!/usr/bin/env python3

import argparse
import bz2
import configparser
import os
import re

import numpy
import matplotlib.cm as cm
import matplotlib.pyplot as plt
import PIL.Image

def apply_colormap(dst_filename, data, colormap):
    image = PIL.Image.fromarray(numpy.uint8(colormap(data)*255))
    image.save(dst_filename)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("input_files", metavar="input-files", nargs="*")

    args = parser.parse_args()

    for filename in args.input_files:
        input_file = configparser.ConfigParser()
        input_file.read(filename)
        width = input_file.getint("config", "width")
        height = input_file.getint("config", "height")

        trajectory_filename = os.path.join(re.sub("\.inp", ".run", filename),
                                           "trajectory")
        bz2_filename = trajectory_filename + ".bz2"
        
        image = None
        
        for colormap_name in ["afmhot", "bone", "gray", "gist_rainbow",
                              "jet", "prism", "gist_ncar",
                              "gist_earth", "BrBG", "PiYG", "seismic",
                              "Accent", "Pastel1", "Set1", "Set3",
                              "Paired", "flag"]:
            dst_filename = re.sub("\.inp", "_{}.png".format(colormap_name),
                                  filename)

            if os.path.exists(dst_filename):
                pass

            if image is None:      
                if os.path.exists(trajectory_filename):
                    data = numpy.fromfile(trajectory_filename,
                                          dtype=numpy.float64)
                elif os.path.exists(bz2_filename):
                    data = numpy.frombuffer(bz2.BZ2File(bz2_filename).read(),
                                            dtype=numpy.float64)
                else:
                    raise(OSError("Could not find trajectory file "
                                  "for {}".format(filename)))
                
                depth = len(data)//(height * width)
                data = data.reshape(height, width, depth)

            t = data[:, :, -1]

            image = PIL.Image.fromarray(
                numpy.uint8(plt.get_cmap(colormap_name)(t)*255))
            image.save(dst_filename)
