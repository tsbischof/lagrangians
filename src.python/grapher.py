import subprocess
import multiprocessing
import os
import ctypes
import configparser
from collections import OrderedDict
import math
import copy
import datetime
import struct

import colormap
import parse
import systems
from c_libraries import lagrangians

class Grapher(object):
    def __init__(self, filename):
        # Use an ordered dictionary to ensure the parameters are
        # passed to the C routines correctly.
        self.filename_base = filename
        if self.filename_base.endswith(".inp"):
            self.filename_base = self.filename_base[:-4]

        self.load(filename)
      
        self.raw_type = ctypes.c_double
        self.restart_type = ctypes.c_byte

    def filename(self, suffix):
        return("{0}.{1}".format(self.filename_base, suffix))

    def load(self, filename):
        """Load options to the grapher from an options object."""
        self.options = parse.Options(filename)
        self.options.load()
        self.run = self.options.run

# The meat of the program. These routines allow us to start all of the work
# for the input file, or just some of it.
    def do_run(self):
        # Fix this eventually to do videos as well; perhaps we want to make
        # snapshots of the trajectory for some number of variables.
        self.do_image()

    def get_work(self):
        n_variables = self.run.points.n_variables
        t_limits = (ctypes.c_double*3)(self.run.t[0], self.run.t[1],\
                                       self.run.t[2])
        height = (ctypes.c_int)(self.run.height)
        width = (ctypes.c_int)(self.run.width)
        integrator = self.run.integrator.function
        rule = self.run.rule.function
        
        result = (self.raw_type*self.run.width)()

        for row_number, r_left, r_right in self.run.points:
            if not self.status[row_number]:
                yield((row_number, r_left, r_right, n_variables, \
                    t_limits, width, integrator, rule, result))
        
    def do_image(self):
        # Are we restarting? If not, allocate the necessary files.
        if self.run.restart:
            self.status_from_restart()
        else:
            self.status = [False for i in range(self.run.height)]
            self.allocate_restart()
            self.allocate_raw()

        pool = multiprocessing.Pool()
        for params in self.get_work():
            self.do_row(params)

    def do_row(self, params):
        row_number, r_left, r_right, n_variables, t_limits, width, \
            integrator, rule, result = params

        print("{0}: Working on row {1} of {2}.".format(\
                        datetime.date.strftime(datetime.datetime.today(), \
                                               "%Y.%m.%d %H:%M:%S"),\
                        row_number, self.run.height-1))

##        print(r_left[0], r_left[1], r_left[2], r_left[3])
##        print(r_right[0], r_right[1], r_right[2], r_right[3])

        lagrangians.do_row(r_left, r_right, n_variables, t_limits,
                        width, integrator, rule, result)
        self.write_row(row_number, result)

    def do_video(self):
        pass

# Routines to allocate and write to the necessary files.
    def allocate_restart(self):
        dst = self.filename("restart")
        if os.path.isfile(dst):
            print("File %s already exists. Remove it or use restart=true."\
                  % self.filename("restart"))
            return(False)
        else:
            with open(self.filename("restart"), "wb") as f:
                for i in range(self.run.height):
                    f.write(self.restart_type(0))

    def allocate_raw(self):
        dst = self.filename("raw")
        if os.path.isfile(dst):
            print("File %s already exists. Remove it or use restart=true."\
                  % self.filename("raw"))
            return(False)
        else:
            with open(self.filename("raw"), "wb") as f:
                for row in range(self.run.height):
                    for column in range(self.run.width):
                        f.write(self.raw_type(0))

    def write_row(self, row_number, row):
        with open(self.filename("raw"), "r+b") as raw_file:
            raw_file.seek(ctypes.sizeof(self.raw_type)*self.run.width*row_number)
            for i in range(self.run.width):
                raw_file.write(struct.pack("d", row[i]))
            raw_file.flush()

        with open(self.filename("restart"), "r+b") as restart_file:
            restart_file.seek(ctypes.sizeof(self.restart_type)*row_number)
            restart_file.write(self.restart_type(1))

        self.status[row_number] = True

    def status_from_restart(self):
        src = self.filename("restart")
        try:
            with open(src, "rb") as f:
                self.status = list()
                for i in range(self.run.height):
                    self.status.append(\
                        bool(f.read(ctypes.sizeof(self.restart_type))))
        except OSError:
            print("Could not open %s for reading." % src)            

    def to_ppm(self, my_colormap=[[0, 0, 0], [255, 0, 0], \
                                  [255, 255, 0],  [255, 255, 255]]):
        """Uses the colormap routine to convert the raw image to something more
aesthetically pleasing."""
        src = self.filename("raw")
        dst = self.filename("ppm")
        time_resolution = self.run.t[1]
        print("Converting %s to %s." % (src, dst))
        colormap.do_colormap(src, dst, self.run.height, self.run.width, \
                             time_resolution, my_colormap)

    def to_png(self):
        """Converts the image to png, using ImageMagick."""
        src = self.filename("ppm")
        dst = self.filename("png")
        if not os.path.isfile(src):
            self.to_ppm()
        print("Converting {0} to {1}.".format(src, dst))
        subprocess.Popen(["convert", src, dst]).wait()
        
if __name__ == "__main__":
    grapher = Grapher("test.inp")
    grapher.do_run()
    grapher.to_ppm()
    grapher.to_png()

