#!/usr/bin/env python3

import os
import shutil
import subprocess

current_dir = os.getcwd()
os.mkdir("blargh")
files = filter(lambda x: x.endswith(".png"), os.listdir(os.getcwd()))
os.chdir("blargh")
cmd = ["ffmpeg", "-i", "%010d.png", "-sameq", "%s_%s.avi" % \
         (os.path.basename(os.path.dirname(current_dir)), \
          os.path.basename(current_dir))]

i = 0
for file in sorted(files):
    print(file)
    shutil.copy(os.path.join("..", file), "%010d.png" % i)
    i += 1

print(cmd)
subprocess.Popen(cmd).wait()
