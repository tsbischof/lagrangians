import subprocess
import sys
import ctypes

from c_libraries import colormapper

##stdio = ctypes.cdll.LoadLibrary("stdlib")

def do_colormap(raw_filename, ppm_filename, height, width, \
                time_resolution, colormap):
    n_points = len(colormap)

    my_colormap = ((ctypes.c_int*3)*n_points)()
    for i in range(n_points):
        for j in range(3):
            my_colormap[i][j] = colormap[i][j]
    print(my_colormap)

    with open(ppm_filename, "w") as ppm_file:
        with open(raw_filename, "rb") as raw_file:
            # systematically add these parameters, particularly the array and
            # the files, until the segfaults stop. Obnoxious, but it should
            # work.
            colormapper.raw_to_ppm(raw_file.fileno(), ppm_file.fileno(),\
                    ctypes.c_double(time_resolution), ctypes.c_int(height), \
                    ctypes.c_int(width), ctypes.c_int(n_points), my_colormap)

if __name__ == "__main__":
    import grapher
    for filename in ["harmonic_oscillator.inp"]:
        my_grapher = grapher.Grapher(filename)
        my_grapher.to_ppm()
        my_grapher.to_png()
