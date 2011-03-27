import subprocess
import multiprocessing
import os
import ctypes
import configparser
from collections import OrderedDict
import math
import copy
import datetime

import colormap
import systems

def xrange(lower, upper, inc=1):
    current = lower
    while current < upper:
        yield(current)
        current += inc

def dict_iter(my_dict, key):
    the_dict = copy.deepcopy(my_dict)
    vals = my_dict[key]
    for val in xrange(vals[0], vals[2], vals[1]):
        the_dict[key] = val
        yield(the_dict)

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

    def load(self):
        """Load options to the grapher from an options object."""
        self.options = parse.Options(filename)
        self.options.load()

# The meat of the program. These routines allow us to start all of the work
# for the input file, or just some of it.
    def get_work(self):
        """For all unfinished rows, return a tuple with the row number and an
iterator which produces the appropriate values for each variable for each
element of the row."""
        # To do: specify the horizontal and vertical rules as a line in phase
        # space, allowing multiple parameters to be varied at once.
        parameters = copy.deepcopy(self.params)
        for row_number, finished in enumerate(self.status):
            if not finished:
                y = self.params[self.plot[1]]
                parameters[self.plot[1]] = y[0] +\
                                           row_number*(y[2]-y[0])/self.height

                yield(row_number, dict_iter(parameters, self.plot[0]))

    def run(self):
        # Are we restarting? If not, allocate the necessary files.
        if self.restart:
            pass
        else:
            pass
            self.allocate_restart()
            self.allocate_raw()     
        for row in self.get_work():
            self.do_row(row)

    def do_row(self, row):
        """Perform the sweep over the variable represented by the given row."""
        row_number, parameters_stream = row
        print("%s: working on row %d." % \
              (datetime.datetime.today().strftime("%Y.%m.%d %H:%M"), \
               row_number))

        result = list()
        for parameters in parmameters_stream:
            pass
        
        # Do this by keeping alive a thread which can be fed a stream of
        # input parameters and spits out a stream of results.
##        out = subprocess.PIPE()
##        subprocess.Popen(["blargh"], stdin=row, stdout=out)

##        result = out.stdout # Basically, have the result piped into an object
        # we can use to write the results to file.

##        self.write_row(result)


# Routines to allocate and write to the necessary files.
    def allocate_restart(self):
        dst = self.filename("restart")
        if os.path.isfile(dst):
            print("File %s already exists. Remove it or use restart=true."\
                  % self.filename("restart"))
            return(False)
        else:
            with open(self.filename("restart"), "wb") as f:
                for i in range(self.height):
                    f.write(self.restart_type(0))

    def allocate_raw(self):
        dst = self.filename("raw")
        if os.path.isfile(dst):
            print("File %s already exists. Remove it or use restart=true."\
                  % self.filename("raw"))
            return(False)
        else:
            with open(self.filename("raw"), "wb") as f:
                for row in range(self.height):
                    for column in range(self.width):
                        f.write(self.raw_type(0))

    def write_row(self, row_number, row):
        with open(self.filename("raw"), "wb") as f:
            f.seek(ctypes.sizeof(self.raw_type)*self.width*row_number)
            for elem in row:
                f.write(self.raw_type(elem))

        with open(self.filename("restart"), "wb") as f:
            f.seek(ctypes.sizeof(self.restart_type)*row_number)
            f.write(self.restart_type(1))

    def do_restart(self):
        src = self.filename("restart")
        try:
            with open(src, "rb") as f:
                self.status = list()
                try:
                    for i in range(self.height):
                        self.status.append(\
                            f.read(ctypes.sizeof(self.restart_type)))
                except:
                    print("Could not read all rows from %s, failed at row %d."%\
                          (src, i))
        except OSError:
            print("Could not open %s for reading." % src)            

    def to_ppm(self):
        """Uses the colormap routine to convert the raw image to something more
aesthetically pleasing."""
        my_colormap = [[0, 0, 0], [255, 0, 0],  [255, 255, 0],  [255, 255, 255]]
        src = self.filename("raw")
        dst = self.filename("ppm")
        time_resolution = self.params["t"][1]
        print("Converting %s to %s." % (src, dst))
        colormap.do_colormap(src, dst, self.height, self.width, \
                             time_resolution, colormap)

    def to_png(self):
        """Converts the image to png, using ImageMagick."""
        src = self.filename("ppm")
        dst = self.filename("png")
        if os.path.isfile(src):
            print("Converting {0} to {1}.".format(src, dst))
            subprocess.Popen(["convert", src, dst]).wait()
        else:
            print("Could not find source file {0} to convert to {1}.".format(\
                                                                    src, dst))

if __name__ == "__main__":
    import parse
    options = parse.Options("test.inp")
    grapher = Grapher(options)
    grapher.run()
