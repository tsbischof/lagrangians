import subprocess
import os
import ctypes
import copy
import datetime
import struct
import re
import bz2
import shutil

import colormap
import parse
import systems
from c_libraries import lagrangians

def compress(filename):
    with open(filename, "rb") as in_file:
        with BZ2File(filename + ".bz2", "wb") as out_file:
            out_file.writelines(in_file)
    os.remove(filename)

def decompress(filename):
    with BZ2File(filename, "rb") as in_file:
        with open(filename[:-4], "wb") as out_file:
            out_file.writelines(in_file)
    os.remove(filename)

class Grapher(object):
    def __init__(self, filename):
        self.filename_base = filename
        if self.filename_base.endswith(".inp"):
            self.filename_base = self.filename_base[:-4]

        self.load(filename)
      
        self.raw_type = ctypes.c_double
        self.restart_type = ctypes.c_int

    def filename(self, suffix=False):
        if not suffix:
            return(self.filename_base)
        else:
            return("{0}.{1}".format(self.filename_base, suffix))

    def load(self, filename):
        """Load options to the grapher from an options object."""
        self.options = parse.Options(filename)
        self.options.load()
        self.run = self.options.run

# The meat of the program. These routines allow us to start all of the work
# for the input file, or just some of it.
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
                    t_limits, width, height, integrator, rule, result))
        
    def do_run(self):
        # Are we restarting? If not, allocate the necessary files.
        if self.run.restart:
            self.status_from_restart()
        else:
            self.status = [False for i in range(self.run.height)]
            if not self.allocate_restart() or not self.allocate_raw():
                return(False)

        for params in self.get_work():
            self.do_row(params)

        self.to_ppm()
        self.to_png()
        compress(self.filename("raw"))
        os.remove(self.filename("ppm"))

    def do_row(self, params):    
        row_number, r_left, r_right, n_variables, t_limits, width, height,\
            integrator, rule, result = params
        print("{0}: Working on row {1} of {2}.".format(\
                    datetime.date.strftime(datetime.datetime.today(), \
                                           "%Y.%m.%d %H:%M:%S"),\
                    row_number, height.value-1))

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
            return(True)

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
            return(True)

    def write_row(self, row_number, row):
        with open(self.filename("raw"), "r+b") as raw_file:
            raw_file.seek(ctypes.sizeof(self.raw_type)*self.run.width*row_number)
            for i in range(self.run.width):
                raw_file.write(self.raw_type(row[i]))

        with open(self.filename("restart"), "r+b") as restart_file:
            restart_file.seek(ctypes.sizeof(self.restart_type)*row_number)
            restart_file.write(self.restart_type(1))

        self.status[row_number] = True

    def get_status(self):
        self.status_from_restart()
        print("{0}: Finished {1} of {2} rows.".format(\
            self.filename(), \
            len(list(filter(lambda x: x, self.status))), \
            self.run.height))

    def status_from_restart(self):
        src = self.filename("restart")
        try:
            with open(src, "rb") as f:
                self.status = list()
                for i in range(self.run.height):
                    val = struct.unpack("<L", \
                            f.read(ctypes.sizeof(self.restart_type)))[0]
                    if val != 0:
                        self.status.append(True)
                    else:
                        self.status.append(False)
        except OSError:
            print("Could not open %s for reading." % src)          

    def to_ppm(self, my_colormap=[[0, 0, 0], [255, 0, 0], \
                                  [255, 255, 0],  [255, 255, 255]]):
        """Uses the colormap routine to convert the raw image to something more
aesthetically pleasing."""
        src = self.filename("raw")
        dst = self.filename("ppm")
        time_resolution = self.run.t[1]
##        print("Converting {0} to {1}.".format(src, dst))
        colormap.do_colormap(src, dst, self.run.height, self.run.width, \
                             time_resolution, my_colormap)

    def to_png(self):
        """Converts the image to png, using ImageMagick."""
        src = self.filename("ppm")
        dst = self.filename("png")
        if not os.path.isfile(src):
            self.to_ppm()
##        print("Converting {0} to {1}.".format(src, dst))
        subprocess.Popen(["convert", src, dst]).wait()

class VideoGrapher(object):
    def __init__(self, filename):
        # Use an ordered dictionary to ensure the parameters are
        # passed to the C routines correctly.
        self.filename_base = filename
        if self.filename_base.endswith(".inp"):
            self.filename_base = self.filename_base[:-4]

        self.load(filename)

        self.folder = "{0}_video".format(self.filename_base)
        if not os.path.isdir(self.folder):
            os.mkdir(self.folder)
      
        self.raw_type = ctypes.c_double
        self.restart_type = ctypes.c_int

    def filename(self, suffix=False):
        if not suffix:
            return(self.filename_base)
        else:
            return("{0}.{1}".format(self.filename_base, suffix))

    def load(self, filename):
        """Load options to the grapher from an options object."""
        self.options = parse.Options(filename)
        self.options.load()
        self.run = self.options.run

    def get_points(self):
        """Allocate and fill the C array representing the initial conditions
for each pixel."""

        points = (ctypes.POINTER(\
                        ctypes.POINTER(self.raw_type))
                  *self.run.height)()

        for row_number, left, right in self.run.points:
            points[row_number] = \
                    (ctypes.POINTER(self.raw_type)*self.run.width)()
##            input()
            for column_number, r in parse.Line(copy.deepcopy(left), \
                                               copy.deepcopy(right), \
                                               self.run.n_variables, \
                                               self.run.width):
                points[row_number][column_number] = \
                     (self.raw_type*self.run.n_variables)()
                for k in range(self.run.n_variables):
                    points[row_number][column_number][k] = r[k]

        return(points)

    def do_run(self):          
        t = self.run.t[0]
        dt = self.run.t[1]
        t_limit = self.run.t[2]

        height = ctypes.c_int(self.run.height)
        width = ctypes.c_int(self.run.width)
        dt_c = ctypes.c_double(dt)
        integrator = self.run.integrator.function

        image = self.get_points()
        step = 0
        self.write_variable_images(image, t)

        while t < t_limit:
            lagrangians.advance_image(image, height, width, dt_c, integrator)
            step += 1
            t += dt 
            if step % self.run.write_every == 0:
                print("{0}: {1}/{2}".format(\
                    datetime.date.strftime(datetime.datetime.today(), \
                                           "%Y.%m.%d %H:%M:%S"),
                    t, t_limit))
                self.write_variable_images(image, t)

        self.make_movie()
        
    def write_variable_images(self, image, t):
        for variable, variable_index in self.run.video:
            self.write_file(variable, variable_index, image, t)

    def write_file(self, variable, variable_index, image, t):
        """Write the raw data to file for the specified variable index."""
        raw_filename = os.path.join(self.folder, \
                                    "{0}-{1}.raw".format(variable, t))
        with open(raw_filename, "wb") \
             as raw_file:
            for i in range(self.run.height):
                for j in range(self.run.width):
                    raw_file.write(\
                        struct.pack("d", image[i][j][variable_index]))

        self.raw_to_png(raw_filename)

    def convert_images(self):
        raw_files = filter(lambda x: x.endswith(".raw.bz2") \
                                     or x.endswith(".raw"), \
                           os.listdir(self.folder))

        variables = dict()
        for raw_filename in raw_files:
            variable, value = raw_filename.split("-")
            if not variable in variables.keys():
                variables[variable] = list()
            variables[variable].append(os.path.join(self.folder, raw_filename))
            
        for variable in sorted(variables.keys()):
            for raw_filename in sorted(variables[variable]):
                print(raw_filename)
                self.raw_to_png(raw_filename)

    def raw_to_png(self, raw_filename):
        if raw_filename.endswith(".bz2"):
            decompress(raw_filename)
            raw_filename = re.sub("\.bz2$", "", raw_filename)
        ppm_filename = re.sub("raw$", "ppm", raw_filename)
        png_filename = re.sub("raw$", "png", raw_filename)
       
        self.raw_to_ppm(raw_filename, ppm_filename)
        self.ppm_to_png(ppm_filename, png_filename)
#        compress(raw_filename)
        os.remove(ppm_filename)
        
    def raw_to_ppm(self, raw_filename, ppm_filename):
        colormap.do_phase_to_image(raw_filename, ppm_filename, \
               self.run.height, self.run.width, \
               [[0, 0, 0], [255, 0, 0], [255, 255, 0], [255, 255, 255], \
                [0, 255, 255], [0, 0, 255], [0, 0, 0]])

    def ppm_to_png(self, ppm_filename, png_filename):
        # Need to find a way to keep the first image from being just black, ImageMagick convert it to 1-bit, which screws up ffmpeg
        subprocess.Popen(["convert", ppm_filename, png_filename]).wait()

    def make_movie(self):
        png_files = filter(lambda x: x.endswith(".png"), \
                           os.listdir(self.folder))
        sort_by_float = lambda x: float(re.search(".-([0-9.]+)\.png", \
                                                  x).groups()[0])

        variables = dict()
        for filename in png_files:
            variable, value = filename.split("-")
            if not variable in variables.keys():
                variables[variable] = list()
            variables[variable].append(filename)

        for variable in sorted(variables.keys()):
            scratch_folder = "{0}_scratch".format(variable)
            if not os.path.isdir(os.path.join(self.folder, scratch_folder)):
                os.mkdir(os.path.join(self.folder, scratch_folder))
            out_format = "%010d.png"
            video_file = "{0}_{1}.avi".format(self.folder, variable)
            
            for number, filename in enumerate(sorted(variables[variable], \
                                                     key=sort_by_float)):
                shutil.copy(os.path.join(self.folder, filename), \
                            os.path.join(self.folder, \
                                         scratch_folder, \
                                         out_format % number))
            subprocess.Popen(["ffmpeg", "-y", "-i", \
                              os.path.join(self.folder, \
                                           scratch_folder,\
                                           "%010d.png"), \
                              "-sameq", \
                              video_file]).wait()
            shutil.rmtree(os.path.join(self.folder, scratch_folder))
