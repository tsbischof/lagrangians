import subprocess
import sys
import ctypes
import os
import re

from c_libraries import colormapper

##stdio = ctypes.cdll.LoadLibrary("stdlib")

def do_colormap(raw_filename, ppm_filename, height, width, \
                time_resolution, colormap):
    n_points = len(colormap)

    my_colormap = (ctypes.POINTER(ctypes.c_int)*n_points)()
    for i in range(n_points):
        my_colormap[i] = (ctypes.c_int*3)()
        for j in range(3):
            my_colormap[i][j] = colormap[i][j]

    colormapper.raw_to_ppm_filenames( \
#            ctypes.create_string_buffer(raw_filename.decode()),\
#            ctypes.create_string_buffer(ppm_filename.decode()),  \
            ctypes.create_string_buffer(raw_filename), \
            ctypes.create_string_buffer(ppm_filename), \
            ctypes.c_double(time_resolution), ctypes.c_int(height), \
            ctypes.c_int(width), ctypes.c_int(n_points), my_colormap)

def do_phase_to_image(raw_filename, height, width):
    colormapper.phase_to_ppm( \
            ctypes.create_string_buffer(raw_filename), \
            ctypes.create_string_buffer(re.sub("raw$", "ppm", raw_filename)), \
            ctypes.c_int(height), ctypes.c_int(width))

if __name__ == "__main__":
    import grapher
    my_grapher = grapher.Grapher("test.inp")
    do_phase_to_image("test_video/x-0.0.raw", 100, 100)
