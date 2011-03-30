import ctypes
import math

from c_libraries import colormapper

def colormap_to_array(colormap):
    n_points = len(colormap)

    my_colormap = (ctypes.POINTER(ctypes.c_int)*n_points)()
    for i in range(n_points):
        my_colormap[i] = (ctypes.c_int*3)()
        for j in range(3):
            my_colormap[i][j] = colormap[i][j]

    return((ctypes.c_int(n_points), my_colormap))

def do_colormap(raw_filename, ppm_filename, height, width, \
                time_resolution, colormap):
    n_points, my_colormap = colormap_to_array(colormap)

    colormapper.raw_to_ppm( \
            ctypes.create_string_buffer(raw_filename), \
            ctypes.create_string_buffer(ppm_filename), \
            ctypes.c_double(time_resolution), \
            ctypes.c_double(0),\
            ctypes.c_int(height), ctypes.c_int(width),\
            n_points, my_colormap)

def do_phase_to_image(raw_filename, ppm_filename, height, width, colormap):
    n_points, my_colormap = colormap_to_array(colormap)
    colormapper.raw_to_ppm( \
            ctypes.create_string_buffer(raw_filename), \
            ctypes.create_string_buffer(ppm_filename), \
            ctypes.c_double(0.001), \
            ctypes.c_double(2*math.pi),\
            ctypes.c_int(height), ctypes.c_int(width), 
            n_points, my_colormap)

if __name__ == "__main__":
    import grapher
    my_grapher = grapher.Grapher("test.inp")
    my_grapher.to_png()
