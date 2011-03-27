import subprocess
import ctypes

colormapper = ctypes.cdll.LoadLibrary("./colormap.so")

def do_colormap(raw_filename, ppm_filename, height, width, \
                time_resolution, colormap):
    n_points = len(colormap)

    with open(ppm_filename, "w") as ppm_file:
        with open(raw_filename, "rb") as raw_file:
            colormapper.raw_to_ppm(raw_file, ppm_file, time_resolution,
                                   height, width, n_points, colormap)

    
