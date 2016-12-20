#!/usr/bin/env python3

import status
import subprocess
import random
import sys
import os
import shutil

def flatten(L):
    result = list()
    for elem in L:
        result.extend(elem)
    return(result)

def random_colormap(n, depth):
    return([random.randint(-depth, depth) for i in range(3*n)])

def do_colormap(raw_filename, png_filename, resolution, height, width, \
                n, colormap, out_width=0):
    command = ["colormap", raw_filename, str(resolution), \
                      "%dx%d" % (height, width), str(n)]
    command.extend(list(map(str, colormap)))
    mapper = subprocess.Popen(command, stdout=subprocess.PIPE)

    if out_width:
        convert_command = ["convert", "ppm:-", "-resize", "%dx" % out_width, \
                           png_filename]
    else:
        convert_command =["convert", "ppm:-", png_filename]

    convert = subprocess.Popen(convert_command, stdin=mapper.stdout).wait()

def do_file(filename, out_filename, n, colormap, unzip=False, new_width=0):
    if unzip:
        subprocess.Popen(["bunzip2", filename[:-4] + ".raw.bz2"]).wait()

    parser = status.read_config(filename)
    height, width = status.get_size(parser)
    resolution = float(parser.get("config", "t").split(",")[1])
    raw_filename = filename[:-4] + ".raw"
    do_colormap(raw_filename, out_filename, resolution, height, width, \
                n, colormap, out_width=new_width)

    if unzip:
        subprocess.Popen(["bzip2", raw_filename]).wait()

##def do_check():
##    # convert all raw files and check that they are still valid.
##    # make sure to enable unzipping, copying to the working directory
##
##    os.chdir("/home/tsbischof/src/lagrangians/run/check/")
##    n = 4
##    colormap = [0, 0, 0,  255, 0, 0,  255, 255, 0, 255, 255, 255]
##    
##    for root, dirs, files in os.walk("/home/tsbischof/src/lagrangians/run"):
##        for file in files:
##            if not "sweep" in root and file.endswith(".raw.bz2"):
##                file = os.path.join(root, file)
##                base_name = os.path.split(file)[1][:-8]
##                shutil.copy(file, base_name + ".raw.bz2")
##                shutil.copy(file[:-8]+".inp", base_name + ".inp")
##                shutil.copy(file[:-8]+".png", base_name + ".png")
##                
##                myfile = base_name + ".inp"
##                print("----------")
##                do_file(myfile, base_name + "_check.png", n, colormap, \
##                        unzip=True)

def do_variety(depth=255, colormaps=None, n=4, out_width=0, overwrite=False):
    # test a variety of colormaps
    os.chdir("/home/tsbischof/src/lagrangians/run/colormap")
    if len(sys.argv) > 1:
        # Use the input file specified
        files = sys.argv[1:]
    else:
        # Look at all input files in this directory.
        files = filter(lambda x: x.endswith(".inp"), os.listdir(os.getcwd()))

    if not colormaps:
        # Did not specify colormaps, get some number of them and use those.
        colormaps = iter(random_colormap(n, depth) for i in range(50))

    for base in files:
        try:
            # Create a folder for each input file
            os.mkdir(base[:-4])
        except: 
            pass

        finished = list()
        if not overwrite:
            # Get all of the old images, so we do not repeat them
            for root, dirs, files in os.walk(base[:-4]):
                finished.extend(files)

        for colormap in colormaps:
            n = len(colormap) // 3
            out_filename = os.path.join(base[:-4], \
                    "%s_%s.png" % (base[:-4], "_".join(map(str, colormap))))
            if not os.path.split(out_filename)[1] in finished:
                print(out_filename)
                do_file(base, out_filename, n, colormap, new_width=out_width)


def get_swatches():
    swatch_dir = "/home/tsbischof/src/lagrangians/run/swatches"
    colormaps = list()
    for root, dirs, files in os.walk(swatch_dir):
        if "/like" in root:
            for file in filter(lambda x: x.endswith(".png"), files):
                colormap = file.split("_")[1:]
                colormap[-1] = colormap[-1].split(".")[0]
                colormap = list(map(int, colormap))
                colormaps.append(colormap)
    return(colormaps)
    
if __name__ == "__main__":
#    os.nice(19)
##    colormap = random_colormap(4, 255)
##    colormap = flatten(reversed([[0, 0, 0],
##                                 [255, 0, 0],
##                                 [255, 255, 0],
##                                 [255, 255, 255]]))
##    colormap = [0,0,0, 255,0,0, 255,255,0, 255,255,255]
##    colormap = [255, 255, 255,  68, 68, 68,  0, 0, 0]
##    colormap = [-146, -98, 29,  217, 208, -192]
##    colormap = flatten([[0xff, 0xff, 0xff],
##                        [0x44, 0x44, 0x44],
##                        [0x00, 0x00, 0x00]])
##    colormap = [0, 0, 0,  255, 0, 0,  255, 255, 0, 255, 255, 255]
##    colormap = [0, 125, 0,  -255, 200, -127,  -150, 255, 255,  255, 100, 100]
##    colormap = [-11, 113, -125, -197, -173, 167, 169, 111, -246, 188, -55, 169]
##    colormap = [21, 146, -232, 41, -52, 77, -131, -71, -44, -55, 137, 209]

#    colormaps = [[255, 255, 255,  68, 68, 68,  0, 0, 0],
#                 [-146, -98, 29,  217, 208, -192]]
#    colormaps = [[255, 255, 255, 0, 0, 0, 255, 0, 0, 255, 255, 0]]
    colormaps = [[17, 27, 24, 78, 67, 75, 48, 62, 88, 36, 89, 157, 62, 63, 75, 185, 124, 80]]

##    colormaps = get_swatches()
    do_variety(colormaps=colormaps, overwrite=False)#, out_width=2000)
##    do_variety(colormaps=colormaps, out_width=1000, overwrite=False)
##    do_variety()
