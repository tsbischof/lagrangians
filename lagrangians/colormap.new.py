#!/usr/bin/env python3

import sys
import configparser
import optparse
import re
import struct
import os
import subprocess
import itertools

def downsample(filename, width):
    name_base = re.sub("\.inp", "", filename)
    
    config = configparser.ConfigParser()
    config.read(filename)
    old_height = config.getint("config", "height")
    old_width = config.getint("config", "width")

    ratio = width/float(old_width)
    height = int(old_height*ratio)

    if os.path.isfile("{0}.raw.new"):
        pass
    else:
        with open("{0}.raw".format(name_base), "rb") as old_raw:
            new_image = [[[0, 0] for i in range(width)]
                         for j in range(height)]

            for i in range(old_height):
                print(name_base, i)
                for j in range(old_width):
                    value = struct.unpack("1d", old_raw.read(struct.calcsize("d")))
                    my_i = int(i*ratio)
                    my_j = int(j*ratio)

                    new_image[my_i][my_j][0] += 1
                    new_image[my_i][my_j][1] += value[0]

        with open("{0}.raw.new", "wb") as new_raw:
            for i in range(old_height):
                for j in range(old_width):
                    pixel = new_image[i][j]
                    new_raw.write(struct.pack("d", pixel[1]/pixel[0]))

        shutil.move("{0}.raw".format(name_base), "{0}.raw.full".format(name_base))
        shutil.move("{0}.raw.new".format(name_base), "{0}.raw".fomrat(name_base))

    return(name_base, width, height)

if __name__ == "__main__":
    parser = optparse.OptionParser()
    parser.add_option("-w", "--width", type=int, help="New width",
                        default=1000)

    options, args = parser.parse_args()


    colormaps = []

    for i in range(255, 10):
        colormaps.append([(0, 0, 0),
                          (0, 0, i),
                          (0, i, i),
                          (i, i, i)])        
    
    for filename in sys.argv[1:]:
        print(filename)
        name_base, new_width, new_height = downsample(filename, options.width)

        try:
            os.mkdir(name_base)
        except:
            pass

        for colormap in colormaps:
            flat = " ".join(itertools.chain(colormap))
            print(flat)
            mapper = subprocess.Popen([
                "colormap",
                "{0}.raw".format(name_base),
                "{0}x{1}".format(new_height, new_width),
                flat],
                                      stdout=subprocess.PIPE)
            converter = subprocess.Popen([
                "convert",
                "ppm:-",
                os.path.join(name_base,
                             "{0}_{1}.png".format(name_base,
                                                  re.sub(" ", "_", flat)))],
                                         stdin=mapper.stdout).wait()
            
