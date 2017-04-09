#!/usr/bin/env python3

import argparse
import bz2
import configparser
import itertools
import os
import re

import numpy
import matplotlib.cm as cm
import matplotlib.colors
import matplotlib.pyplot as plt
import PIL.Image

system_variables = {
     "dangling_stick": ["r", "dr", "phi1", "dphi1", "phi2", "dphi2"],
     "double_pendulum": ["phi1", "dphi1", "phi2", "dphi2"],
     "double_spring": ["phi1", "dphi1", "phi2", "dphi2", "r1", "dr1", "r2", "dr2"],
     "double_well": ["x", "v"],
     "springy_pendulum": ["r", "dr", "phi1", "dphi1", "phi2", "dphi2"],
     "trig": ["x", "v"]
                    }
#

def apply_colormap(dst_filename, data, colormap):
    image = PIL.Image.fromarray(numpy.uint8(colormap(data)*255))
    image.save(dst_filename)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--all", action="store_true")
    parser.add_argument("input_files", metavar="input-files", nargs="*")
    parser.add_argument("--flip", action="store_true")
    parser.add_argument("--force-creation", action="store_true")

    args = parser.parse_args()

    if args.all:
        colormap_names = ["afmhot", "bone", "gray", "gist_rainbow",
                          "jet", "prism", "gist_ncar",
                          "gist_earth", "BrBG", "PiYG", "seismic",
                          "Accent", "Pastel1", "Set1", "Set3",
                          "Paired", "flag"]
    else:
        colormap_names = ["afmhot"]

    for filename in args.input_files:
        input_file = configparser.ConfigParser()
        input_file.read(filename)
        width = input_file.getint("config", "width")
        height = input_file.getint("config", "height")
        variables = system_variables[input_file.get("config", "system")] + ["t"]

        trajectory_filename = os.path.join(re.sub("\.inp", ".run", filename),
                                           "trajectory")
        bz2_filename = trajectory_filename + ".bz2"
        
        data = None
        
        for index, variable in enumerate(variables):
            for colormap_name in colormap_names:
                dst_filename = os.path.join(re.sub("\.inp", ".run", filename),
                                            "{}_{}.png".format(variable, colormap_name))

                if os.path.exists(dst_filename) and not args.force_creation:
                    continue

                if data is None:      
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
                    if args.flip:
                        raw = data
                        data = numpy.zeros((width, width, depth))
                        data[height:, :, :] = raw
                        data[:height, :, :-1] = \
                                      -numpy.rot90(raw, k=2)[:, :, :-1]
                        data[:height, :, -1] = numpy.rot90(raw, k=2)[:, :, -1]

                color = plt.get_cmap(colormap_name)(
                    matplotlib.colors.Normalize()(data[:, :, index]),
                    bytes=True)
                color[:, :, 3] = 255
                PIL.Image.fromarray(color).save(dst_filename)
