#!/usr/bin/env python3

import argparse
import bz2
import configparser
import itertools
import math
import os
import re

import numpy
import matplotlib.cm as cm
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
colormap_name = "afmhot"

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
        t_start, t_stop, t_step = map(float, input_file.get("config", "t").split(","))
        frames = int(math.floor((t_stop - t_start)/t_step)) + 1
        variables = system_variables[input_file.get("config", "system")]

        trajectory_filename = os.path.join(re.sub("\.inp", ".run", filename),
                                           "trajectory")
        bz2_filename = trajectory_filename + ".bz2"
        
        data = None
        
        for index, variable in enumerate(variables):
            for frame in range(frames):
                dst_filename = os.path.join(re.sub("\.inp", ".run", filename),
                       "{0}_{1}_{2:08d}.png".format(variable, colormap_name, frame))

                if os.path.exists(dst_filename):
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

                    data = data.reshape(frames, height, width, len(variables))

                image = PIL.Image.fromarray(numpy.uint8(
                          plt.get_cmap(colormap_name)(data[frame, :, :, index])*255))
                image.save(dst_filename)
