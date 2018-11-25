#!/usr/bin/env python3

import argparse
import bz2
import collections
import configparser
import itertools
import logging
import math
import os
import re
import subprocess

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
     "lotka": ["x", "y"],
     "springy_pendulum": ["r", "dr", "phi1", "dphi1", "phi2", "dphi2"],
     "trig": ["x", "v"]
                    }
#

def apply_colormap(dst_filename, data, colormap):
    image = PIL.Image.fromarray(numpy.uint8(colormap(data)*255))
    image.save(dst_filename)

def trajectory_exists(filename):
    return(os.path.exists(filename) or os.path.exists(filename + ".bz2"))

def scale_dimension(d, scale):
    return(int(d*scale))

def scale_data(trajectory_filename, scale, height, width, depth,
               scratch_dir="/tmp"):
    bz2_filename = trajectory_filename + ".bz2"
    dst_height = scale_dimension(height, scale)
    dst_width = scale_dimension(height, scale)
    dst_trajectory_filename = trajectory_filename + ".{}x{}".format(dst_height,
                                                                    dst_width)

    if not trajectory_exists(dst_trajectory_filename):
        # create working files
        logging.info("copying images to scratch space")
        dst_files = collections.deque()
        for index in range(depth):
            dst_filename = os.path.join(scratch_dir, str(index))
            dst_files.append((dst_filename,
                              open(dst_filename, "wb")
                              ))

        if os.path.exists(trajectory_filename):
            stream_in = open(trajectory_filename, "rb")
        elif os.path.exists(bz2_filename):
            stream_in = bz2.open(bz2_filename, "rb")
        else:
            raise(OSError("Could not open {}".format(trajectory_filename)))

        for n in range(height*width*depth):
##            if n % (100*height*depth) == 0:
##                print(n)
            dst_files[0][1].write(stream_in.read(8))
            dst_files.rotate(-1)

        # resize working files
        logging.info("resizing images")
        dst_filenames = list()
        for src_filename, fh in dst_files:
            fh.close()
            dst_filename = src_filename + ".{}x{}".format(dst_height, dst_width)
            dst_filenames.append(dst_filename)
            subprocess.Popen(["convert",
                              "-size", "{}x{}".format(height, width),
                              "-define", "quantum:format=floating-point",
                              "-endian", "LSB",
                              "-depth", "64",
                              "gray:{}".format(src_filename),
                              "-resize", "{}x{}".format(dst_height, dst_width),
                              "gray:{}".format(dst_filename)]).wait()
            
        # rebuild trajectory
        data = numpy.zeros((dst_height, dst_width, depth))
        for index, filename in enumerate(sorted(dst_filenames)):
            raw = numpy.fromfile(filename, dtype=numpy.float64)
            data[:, :, index] = raw.reshape((dst_height, dst_width))

        data.tofile(dst_trajectory_filename)
    
    return(dst_trajectory_filename, (dst_height, dst_width))

def load_data(trajectory_filename, height, width, depth,
              flip=False, scale=None):
    if scale:
        trajectory_filename, (height, width) = \
                             scale_data(trajectory_filename, scale,
                                        height, width, depth)
    
    bz2_filename = trajectory_filename + ".bz2"
    
    if os.path.exists(trajectory_filename):
        data = numpy.fromfile(trajectory_filename,
                          dtype=numpy.float64)
    elif os.path.exists(bz2_filename):
        data = numpy.frombuffer(bz2.BZ2File(bz2_filename).read(),
                            dtype=numpy.float64)
    else:
        raise(OSError("Could not find trajectory file "
                      "for {}. Have: {}".format(filename, trajectory_filename)))

    depth = len(data)//(height * width)
    data = data.reshape(height, width, depth)
    if args.flip:
        raw = data
        data = numpy.zeros((width, width, depth))
        data[height:, :, :] = raw
        data[:height, :, :-1] = \
                      -numpy.rot90(raw, k=2)[:, :, :-1]
        data[:height, :, -1] = numpy.rot90(raw, k=2)[:, :, -1]

    return(data)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    parser = argparse.ArgumentParser()
    parser.add_argument("--all", action="store_true")
    parser.add_argument("input_files", metavar="input-files", nargs="*")
    parser.add_argument("--flip", action="store_true")
    parser.add_argument("--force-creation", action="store_true")
    parser.add_argument("--colormap")
    parser.add_argument("--scale", type=float)

    args = parser.parse_args()

    if args.all:
        colormap_names = ["afmhot", "bone", "gray", "gist_rainbow",
                          "jet", "prism", "gist_ncar",
                          "gist_earth", "BrBG", "PiYG", "seismic",
                          "Accent", "Pastel1", "Set1", "Set3",
                          "Paired", "flag"]
    elif args.colormap:
        colormap_names = [args.colormap]
    else:
        colormap_names = ["afmhot"]

    for filename in args.input_files:
        input_file = configparser.ConfigParser()
        input_file.read(filename)
        width = input_file.getint("config", "width")
        height = input_file.getint("config", "height")
        variables = system_variables[input_file.get("config", "system")] + \
                    ["t"]

        trajectory_filename = os.path.join(re.sub("\.inp", ".run", filename),
                                           "trajectory")            
        bz2_filename = trajectory_filename + ".bz2"

        data = None
        
        for (index, variable), colormap_name in itertools.product(
            enumerate(variables), colormap_names):
            if args.scale:
                image_dir = "images_{}x{}".format(
                    scale_dimension(height, args.scale),
                    scale_dimension(width, args.scale))
            else:
                image_dir = "images"

            dst_dir = os.path.join(re.sub("\.inp", ".run", filename),
                                        image_dir)
            try:
                os.makedirs(dst_dir)
            except:
                pass
            
            dst_filename = os.path.join(dst_dir, 
                                        "{}_{}.png".format(variable,
                                                           colormap_name))

            if os.path.exists(dst_filename) and not args.force_creation:
                continue

            if data is None:
                data = load_data(trajectory_filename,
                                 height, width, len(variables),
                                 flip=args.flip, scale=args.scale)
                
            color = plt.get_cmap(colormap_name)(
                matplotlib.colors.Normalize()(data[:, :, index]),
                bytes=True)
            color[:, :, 3] = 255
            PIL.Image.fromarray(color).save(dst_filename)
