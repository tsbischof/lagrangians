import math
import struct
import sys
import tempfile
import status
import subprocess

def restart_to_png(filename):
    base = filename[:-8]
    parser = status.read_config(base + ".inp")
    height, width = status.get_size(parser)
    t_limits = status.get_t_limits(parser)
    depth = t_limits[2]-t_limits[0]
    
    ppm_file = open(base+".ppm", "w")
    restart_file = open(filename, "rb")

    print("Writing ppm file.")
    restart_file.seek(height*4)
    scale = math.ceil(depth/3)
    ppm_file.writelines(["P3\n", "%d %d\n" % (width, height), "%d\n" % scale])

    for i in range(height):
        if i % (math.floor(height)/20) == 0:
            print("On row %d of %d." % (i, height))
        for j in range(width):
            val = struct.unpack("d", restart_file.read(8))[0]
            ppm_file.write(" ".join(choose_ppm(val, depth, scale)))
            ppm_file.write("  ")
        ppm_file.write("\n")

    print("Converting ppm to png.")
    subprocess.Popen(["convert", base+".ppm", base+".png"])

def choose_ppm(val, depth, scale):
    rgb = [0 for i in range(3)]
    val = math.ceil(val/depth*((scale*3)+2))
    for i in range(3):
        if val < 0:
            rgb[i] = 0
        elif val > scale:
            rgb[i] = math.floor(scale)
        else:
            rgb[i] = val

        val -= scale+1

    return(list(map(str, rgb)))

if __name__ == "__main__":
    filenames = sys.argv[1:]
    for filename in filter(lambda x: x.endswith(".restart"), filenames):
        print(filename)
        restart_to_png(filename)
