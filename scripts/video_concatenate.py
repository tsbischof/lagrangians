#!/usr/bin/env python3

import itertools
import os
import re
import subprocess

parser = re.compile("(?P<variable>[a-z0-9]+)_(?P<colormap>[^_]+)"
                    "_(?P<frame>[0-9]{8})\.png")
frames = list(filter(parser.search, os.listdir(os.getcwd())))
variables = set(map(lambda _: parser.search(_).group("variable"), frames))
colormaps = set(map(lambda _: parser.search(_).group("colormap"), frames))

for variable, colormap in itertools.product(variables, colormaps):
    if not os.path.exists("{}_{}_00000000.png".format(variable, colormap)):
        continue

    dst_filename = "{}_{}.mp4".format(variable, colormap)

    if not os.path.exists(dst_filename):
        subprocess.Popen(["ffmpeg", "-i", 
                          "{0}_{1}_%08d.png".format(variable, colormap),
                          "-framerate", "25",
                          dst_filename]).wait()
