import subprocess
import os
import ctypes
import copy
import datetime
import struct
import re
import shutil

from lagrangians import colormap, options, clib

class Grapher(object):
    def __init__(self, filename):
        self.name_base = filename
        if self.name_base.endswith(".inp"):
            self.name_base = self.filename_base[:-4]

        self.options = parse.Options(filename)
      
        self.raw_type = ctypes.c_double
        self.restart_type = ctypes.c_int

    def filename(self, suffix=False):
        if not suffix:
            return(self.filename_base)
        else:
            return("{0}.{1}".format(self.filename_base, suffix))

# The meat of the program. These routines allow us to start all of the work
# for the input file, or just some of it.
    def get_work(self):
        n_variables = self.options.points.n_variables
        t_limits = (ctypes.c_double*3)(self.options.t[0], self.options.t[1],\
                                       self.options.t[2])
        height = (ctypes.c_int)(self.options.height)
        width = (ctypes.c_int)(self.options.width)
        integrator = self.options.integrator.function
        rule = self.options.rule.function
        
        result = (self.raw_type*self.options.width)()

        for row_number, points in enumerate(self.options.points):
            r_left, r_right = points
            if not self.status[row_number]:
                yield((row_number, r_left, r_right, n_variables, \
                    t_limits, width, height, integrator, rule, result))
        
    def do_run(self):
        # Are we restarting? If not, allocate the necessary files.
        if self.options.restart:
            self.status_from_restart()
        else:
            self.status = [False for i in range(self.options.height)]
            if not self.allocate_restart() or not self.allocate_raw():
                return(False)

        for params in self.get_work():
            self.do_row(params)

        self.to_png()
        utils.compress(self.filename("raw"))
        os.remove(self.filename("ppm"))

    def do_row(self, params):    
        row_number, r_left, r_right, n_variables, t_limits, width, height,\
            integrator, rule, result = params

        if self.options.extend_time:
            print("Extension of time is not yet implemented.")
            
        print("{0}: Working on row {1} of {2}.".format(\
                    datetime.date.strftime(datetime.datetime.today(), \
                                           "%Y.%m.%d %H:%M:%S"),\
                    row_number, height.value-1))

        print([a for a in r_left])

        lagrangians.do_row(r_left, r_right, n_variables, t_limits,
                        width, integrator, rule, result)
        self.write_row(row_number, result)

# Routines to allocate and write to the necessary files.
    def allocate_restart(self):
        dst = self.filename("restart")
        if os.path.isfile(dst):
            print("File %s already exists. Remove it or use restart=true."\
                  % self.filename("restart"))
            return(False)
        else:
            with open(self.filename("restart"), "wb") as f:
                for i in range(self.options.height):
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
                for row in range(self.options.height):
                    for column in range(self.options.width):
                        f.write(self.raw_type(0))
            return(True)

    def write_row(self, row_number, row):
        with open(self.filename("raw"), "r+b") as raw_file:
            raw_file.seek(ctypes.sizeof(self.raw_type)\
                          *self.options.width*row_number)
            for i in range(self.options.width):
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
            self.options.height))

    def status_from_restart(self):
        src = self.filename("restart")
        try:
            with open(src, "rb") as f:
                self.status = list()
                for i in range(self.options.height):
                    val = struct.unpack("<L", \
                            f.read(ctypes.sizeof(self.restart_type)))[0]
                    if val != 0:
                        self.status.append(True)
                    else:
                        self.status.append(False)
        except OSError:
            print("Could not open %s for reading." % src)

    def to_ppm(self, my_colormap=[[0, 0, 0], [255, 0, 0], \
                                  [255, 255, 0],  [255, 255, 255]], \
               src=None, dst=None):
        """Uses the colormap routine to convert the raw image to something more
aesthetically pleasing."""
        if src == None:
            src = self.filename("raw")
        if dst == None:
            dst = self.filename("ppm")
        time_resolution = self.options.t[1]
        colormap.do_colormap(src, dst, self.options.height, self.options.width, \
                             time_resolution, my_colormap)

    def to_png(self, my_colormap=[[0, 0, 0], [255, 0, 0], \
                                  [255, 255, 0],  [255, 255, 255]],
               src=None, dst=None, width=None):
        """Converts the image to png, using ImageMagick."""
        if src == None:
            src = self.filename("ppm")

        if dst == None:    
            dst = self.filename("png")

        if not os.path.isfile(src):
            self.to_ppm(my_colormap, self.filename("raw"), self.filename("ppm"))
        
        if width:
            cmd = ["convert", src, "-resize", "%dx" % width, dst]
        else:
            cmd = ["convert", src, dst]

        subprocess.Popen(cmd).wait()





class VideoGrapher(object):
    def __init__(self, filename):
        # Use an ordered dictionary to ensure the parameters are
        # passed to the C routines correctly.
        self.filename_base = filename
        if self.filename_base.endswith(".inp"):
            self.filename_base = self.filename_base[:-4]

        self.options = parse.Options(filename)

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

    def get_points(self):
        """Allocate and fill the C array representing the initial conditions
for each pixel."""
        points = (ctypes.POINTER(\
                        ctypes.POINTER(self.raw_type))
                  *self.options.height)()

        for row_number, point in enumerate(self.options.points):
            left, right = point
            points[row_number] = \
                    (ctypes.POINTER(self.raw_type)*self.options.width)()
            for column_number, r in enumerate(parse.Line(copy.deepcopy(left), \
                                               copy.deepcopy(right), \
                                               self.options.n_variables, \
                                               self.options.width)):
                points[row_number][column_number] = \
                     (self.raw_type*self.options.n_variables)()
                for k in range(self.options.n_variables):
                    points[row_number][column_number][k] = r[k]

        return(points)

    def do_run(self):          
        t = self.options.t[0]
        dt = self.options.t[1]
        t_limit = self.options.t[2]

        height = ctypes.c_int(self.options.height)
        width = ctypes.c_int(self.options.width)
        dt_c = ctypes.c_double(dt)
        integrator = self.options.integrator.function

        image = self.get_points()
        step = 0
        self.write_variable_images(image, t)

        while t < t_limit:
            lagrangians.advance_image(image, height, width, dt_c, integrator)
            step += 1
            t += dt 
            if step % self.options.write_every == 0:
                print("{0}: {1}/{2}".format(\
                    datetime.date.strftime(datetime.datetime.today(), \
                                           "%Y.%m.%d %H:%M:%S"),
                    t, t_limit))
                self.write_variable_images(image, t)

        self.make_movie()
        
    def write_variable_images(self, image, t):
        for variable, variable_index in self.options.video:
            self.write_file(variable, variable_index, image, t)

    def write_file(self, variable, variable_index, image, t):
        """Write the raw data to file for the specified variable index."""
        raw_filename = os.path.join(self.folder, \
                                    "{0}-{1}.raw".format(variable, t))
        with open(raw_filename, "wb") \
             as raw_file:
            for i in range(self.options.height):
                for j in range(self.options.width):
                    raw_file.write(\
                        struct.pack("d", image[i][j][variable_index]))

        self.raw_to_png(raw_filename)

    def raw_to_png(self, raw_filename):
        if raw_filename.endswith(".bz2"):
            decompress(raw_filename)
            raw_filename = re.sub("\.bz2$", "", raw_filename)
        ppm_filename = re.sub("raw$", "ppm", raw_filename)
        png_filename = re.sub("raw$", "png", raw_filename)

        # raw to ppm
        colormap.do_phase_to_image(raw_filename, ppm_filename, \
               self.options.height, self.options.width, \
               [[0, 0, 0], [255, 0, 0], [255, 255, 0], [255, 255, 255], \
                [0, 255, 255], [0, 0, 255], [0, 0, 0]])

        # ppm to png
        subprocess.Popen(["convert", ppm_filename, png_filename]).wait()

        # remove the raw and ppm files to save space
        os.remove(ppm_filename)
        os.remove(raw_filename)
        
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

if __name__ == "__main__":
    grapher = Grapher("test.inp")
    grapher.do_run()
