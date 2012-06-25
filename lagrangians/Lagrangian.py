import subprocess
import os
import ctypes
import copy
import datetime
import struct
import re
import shutil
import logging

from lagrangians import colormap, options, clib, modes, types

class Lagrangian(object):
    def __init__(self, filename):
        self.name_base = filename
        if self.name_base.endswith(".inp"):
            self.name_base = self.filename_base[:-4]

        self.options = options.Options(filename)
      
        types.raw_type = "d"
        types.restart_type = "i"

        self._status = None

    def filename(self, suffix=False):
        if suffix:
            return("{0}.{1}".format(self.name_base, suffix))
        else:
            return(self.name_base)

    def colormap(self, colormap=[]):
        raise(ValueError("Colormap not yet implemented."))

    def rows(self):
        """Based on the chosen options, returns row iterators which can
be used to determine the initial conditions for each pixel."""
        for row_number, row in enumerate(self.options.rows):
            if not self.status(row_number):
                yield(row)

    def image_row(self, row_number, r_left, r_right, n_variables,
                  t_limits, width, height,
                  integrator, rule, row):
        pass


    def allocate_restart(self):
        restart_filename = self.filename("restart")
        if os.path.isfile(dst):
            raise(IOError("File {0} already exists. "
                          "Remove it or use restart=true.".format(
                              restart_filename)))
        else:
            with open(restart_filename, "wb") as restart_file:
                restart_file.write(struct.pack("{0}{1}".format(
                    self.options.height,
                    types.restart_type)))

    def allocate_raw(self):
        raw_filename = self.filename("raw")
        if os.path.isfile(dst):
            raise(IOError("File {0} already exists. "
                          "Remove it or use restart=true.".format(
                              raw_filename)))
        else:
            with open(raw_filename, "wb") as raw_file:
                raw_file.write(struct.pack("{0}{1}".format(
                    self.options.height*self.options.width,
                    types.raw_type)))

    def write_row(self, row_number, row):
        with open(self.filename("raw"), "r+b") as raw_file:
            raw_file.seek(
                struct.calcsize(
                    "{0}{1}".format(
                        self.options.width*row_number,
                        types.raw_type)))

            raw_file.write(
                struct.pack(
                    "{0}{1}".format(
                        self.options.width,
                        types.raw_type),
                    bytes(row)))

        with open(self.filename("restart"), "r+b") as restart_file:
            restart_file.seek(
                struct.calcsize(
                    "{0}{1}".format(
                        row_number,
                        types.restart_type)))
            restart_file.write(
                struct.pack(types.restart_type,
                            1))

        self._status[row_number] = True

    def status(self, row_number=None):
        if not self._status:
            self._status = []
            with open(self.filename("restart"), "rb") as restart_file:
                self._status = map(lambda x: x == 1,
                                   struct.unpack(
                                       "{0}{1}".format(
                                           self.options.height,
                                           types.restart_type)))

        if row_number:
            return(self._status[row_number])
        else:
            logging.info(
                "{0}: Finished {1} of {2} rows.".format(
                    self.filename(),
                    len(filter(lambda x: x, self.status)),
                    self.options.height))

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

    def run(self):
        if self.options.mode == modes.MODE_IMAGE:
            self.run_image()
        elif self.options.mode == modes.MODE_VIDEO:
            self.run_video()
        else:
            raise(ValueError(
                "Cannot act on mode {0}".format(self.options.mode)))

    def run_image(self):
        if self.options.restart:
            self.status_from_restart()
        else:
            self.status = [False for i in range(self.options.height)]
            self.allocate_restart()
            self.allocate_raw()

        if self.options.extend_time:
            raise(ValueError("Extension of time is not yet implemented."))

        n_variables = ctypes.c_int(self.options.n_variables)
        t_limits = (ctypes.c_double*3)(self.options.t[0],
                                       self.options.t[1],
                                       self.options.t[2])
        width = ctypes.c_int(self.options.width)
        integrator = self.options.integrator.function
        rule = self.options.rule.function
        workspace = (ctypes.c_double*self.options.width)()

        for row_number, row in enumerate(self.rows()):
            logging.info("{0}: Working on row {1} of {2}.".format(
                datetime.date.strftime(datetime.datetime.today(),
                                       "%Y.%m.%d %H:%M:%S"),
                row_number, height.value-1))

            r_left, r_right = row

            lagrangians.do_row(r_left, r_right, n_variables, t_limits,
                            width, integrator, rule, workspace)
            
        self.write_row(row_number, row)
            
        for row in self.rows():
            self.do_row(row)

        self.to_png()

    def run_video(self):
        raise(ValueError("Video mode not yet enabled."))





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
      
        types.raw_type = ctypes.c_double
        types.restart_type = ctypes.c_int

    def filename(self, suffix=False):
        if not suffix:
            return(self.filename_base)
        else:
            return("{0}.{1}".format(self.filename_base, suffix))

    def get_points(self):
        """Allocate and fill the C array representing the initial conditions
for each pixel."""
        points = (ctypes.POINTER(\
                        ctypes.POINTER(types.raw_type))
                  *self.options.height)()

        for row_number, point in enumerate(self.options.points):
            left, right = point
            points[row_number] = \
                    (ctypes.POINTER(types.raw_type)*self.options.width)()
            for column_number, r in enumerate(parse.Line(copy.deepcopy(left), \
                                               copy.deepcopy(right), \
                                               self.options.n_variables, \
                                               self.options.width)):
                points[row_number][column_number] = \
                     (types.raw_type*self.options.n_variables)()
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
    a = Lagrangians("test.inp")
    a.run()
