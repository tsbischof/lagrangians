#!/usr/bin/env python3

import os
import re
import sys

from matplotlib import gridspec
import matplotlib.pyplot as plt
import numpy

import configparser

for input_filename in sys.argv[1:]:
    config = configparser.ConfigParser()
    config.read(input_filename)

    height = config.getint("config", "height")
    width = config.getint("config", "width")

    data = numpy.fromfile(os.path.join(re.sub("\.inp", ".run", input_filename),
                                       "trajectory"),
                          dtype=numpy.float64)
    depth = round(len(data)/(height*width))
    data = data.reshape(height, width, depth)

    fig = plt.figure()
    gs = gridspec.GridSpec(int(numpy.ceil(numpy.sqrt(depth))),
                           int(numpy.ceil(numpy.sqrt(depth))))

    for depth in range(data.shape[2]):
        ax = fig.add_subplot(gs[depth])
        ax.imshow(data[:, :, depth], interpolation="none", origin="lower left")
        
    fig.tight_layout()
    plt.show(fig)
    plt.close(fig)
