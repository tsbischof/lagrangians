import subprocess
import sys
import ctypes
import os

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

    colormapper.raw_to_ppm_filenames(ctypes.create_string_buffer(raw_filename),\
            ctypes.create_string_buffer(ppm_filename),  \
            ctypes.c_double(time_resolution), ctypes.c_int(height), \
            ctypes.c_int(width), ctypes.c_int(n_points), my_colormap)

if __name__ == "__main__":
    import grapher
    for filename in ["harmonic_oscillator.inp"]:
        my_grapher = grapher.Grapher(filename)
        my_grapher.to_ppm()
        my_grapher.to_png()
