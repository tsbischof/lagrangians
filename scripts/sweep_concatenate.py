#!/usr/bin/env python3

import itertools
import os
import re
import subprocess

parser = re.compile("(?P<variable>[a-z0-9]+)_(?P<colormap>[^_]+)\.png")

variables = set()
colormaps = set()

for run in filter(lambda _: _.endswith(".run"), os.listdir(os.getcwd())):
    frames = list(filter(parser.search, os.listdir(run)))
    variables.update(set(map(lambda _: parser.search(_).group("variable"),
                             frames)))
    colormaps.update(set(map(lambda _: parser.search(_).group("colormap"),
                             frames)))

for variable, colormap in itertools.product(variables, colormaps):
    dst_filename = "{}_{}.mp4".format(variable, colormap)

    if not os.path.exists(dst_filename):
        subprocess.Popen(["ffmpeg", "-i", 
                          "%05d.run/{0}_{1}.png".format(variable, colormap),
                          "-framerate", "25",
                         dst_filename]).wait()
