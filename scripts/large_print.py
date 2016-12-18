#!/usr/bin/env python

import subprocess
import os
import re

class Point:
    def __init__(self, x, y):
        self.x = int(x)
        self.y = int(y)

def crop_image(src_filename, dst_filename, crop_size, offset=None,
               out_size=None):
    cmd = ["convert", src_filename, "+repage", "-crop"]
    
    crop = "{0}x{1}".format(crop_size.x, crop_size.y)
    if offset:
        crop += "+{0}+{1}".format(offset.x, offset.y)
    cmd.append(crop)

    if out_size:
        cmd.extend(["-resize", "{0}x{1}".format(out_size.x, out_size.y)])

    cmd.append(dst_filename)

##    print(" ".join(cmd))
    subprocess.Popen(cmd).wait()

def image_dimensions(filename):
    process = subprocess.Popen(["identify", "-format", "%w,%h", filename],
                               stdout=subprocess.PIPE)
    dimensions = list(map(int, process.stdout.read().strip().split(",")))
    return(Point(dimensions[0], dimensions[1]))
    

if __name__ == "__main__":
    srcs = ["double_spring/double_spring00000014_255_255_255_0_0_0_255_0_0_255_255_0.png"]
    dst_folder_root = "prints/large"

    # make the program center the resulting image: equal border on all sides?
    # Size to source from for a page
    # 300dpi on letter paper, 0.5" margin
    dpi = 300
    rel_height = 7.5
    rel_width = 10.5
    shrink_ratio = 4 # this many pixels of the original image are used for the
                     # final image
    
    page_dimensions = Point(dpi*rel_width, dpi*rel_height)
    frame_dimensions = Point(dpi*rel_width*shrink_ratio,
                             dpi*rel_height*shrink_ratio)
    # Size of a single page for printing

    for src in srcs:
        src_dimensions = image_dimensions(src)

        root, name = os.path.split(src)
        name_base = re.sub("\.[^\.]+$", "", name)
        dst_folder = os.path.join(dst_folder_root, name_base)
        try:
            os.makedirs(dst_folder)
        except:
            if os.path.isdir(dst_folder):
                pass
            else:
                raise(ValueError("Could not create directory {0}".format(dst_folder)))

        for index_x, origin_x in enumerate(range(0, src_dimensions.x,
                                                 frame_dimensions.x)):
            for index_y, origin_y  in enumerate(range(0, src_dimensions.y,
                                                      frame_dimensions.y)):
                dst = os.path.join(dst_folder,
                            "{0}_{1:03d}_{2:03d}.png".format(name_base,
                                                index_x, index_y))

                print("{0} --> {1}".format(src, dst))
                crop_image(src, dst, frame_dimensions,
                          offset=Point(origin_x, origin_y),
                            out_size=page_dimensions)
                
        
