#!/usr/bin/env python3
import os
import subprocess
import shutil

wallpaper_root = "/home/tsbischof/Pictures/Wallpapers/slideshow"

wallpapers = os.listdir(wallpaper_root)

prefixes = ["springy_pendulum", "double_pendulum", "double_spring", \
            "dangling_stick"]

sources = list()

def contained(L, elem):
    for myelem in L:
        if myelem in elem:
            return(True)
    return(False)

for root, dirs, files in os.walk(os.getcwd()):
    for file in files:
        for prefix in prefixes:
            if "png" in file and prefix in file \
               and not contained(["sweep", "small", "blargh", "check", \
                                  "colormap", "swatches"], root) \
               and not contained(["new"], file) \
               and not file in wallpapers:
                sources.append([root, file])
                params = str(subprocess.check_output(["file", \
                                               os.path.join(root, file)]))
                size = list(map(int, params.split(",")[1].split("x")))
                src = os.path.join(root, file)
                dst = os.path.join(wallpaper_root, file)

                print(src, "->", dst)               
                
                if size[0] < size[1]:
                    print("Rotating...")
                    subprocess.Popen(["convert", "-rotate", "90", \
                                      "-resize", "x1080", src, dst]).wait()
                else:
                    subprocess.Popen(["convert", "-resize", "x1080", \
                                      src, dst]).wait()
                    
