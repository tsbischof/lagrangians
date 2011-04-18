import configparser
import ctypes
import os
import tempfile
from collections import OrderedDict

import systems

##def list_to_array(array, L):
##    for i in range(len(L)):
##        result[i] = L[i]
##    return(result)

def sign(x):
    return(x/abs(x))

def increment_array(array, dr):
    for index, elem in enumerate(dr):
        array[index] += elem

def is_video(filename):
    parser = configparser.SafeConfigParser()
    parser.read(filename)
    return(parser.has_option("config", "video"))

class Points(object):
    def __init__(self, horizontal=None, vertical=None, defaults=None, \
                 params=None, height=100, width=100):
        self.height = height
        self.width = width

        self.left, self.right, self.vertical_dr = \
                   self.from_config(horizontal, vertical, \
                                    defaults, params, height)
        self.n = -1
        self.n_variables = len(params.keys())

    def from_config(self, horizontal, vertical, defaults, params, size):
        """Fill the beginning and end of the line with the values from
variables, taken from the configuration file. For those which are not directly
specified, the values are taken from defaults. Anything not specified in the
input file gets taken from params, which are set externally."""
        n_variables = len(params.keys())
        left = (ctypes.c_double*n_variables)()
        right = (ctypes.c_double*n_variables)()
        upper = (ctypes.c_double*n_variables)()
        vertical_dr = (ctypes.c_double*n_variables)()

        for key in horizontal.keys():
            if key in vertical.keys():
                print("Cannot specify the same variable on both axes: {0}.".\
                      format(key))
            if key in defaults.keys():
                print(\
            "Cannot specify the same variable on an axis and a default: {0}.".\
            format(key))

        # First, build the values which actually vary. 
        for index, key in enumerate(params.keys()):
            if key in horizontal.keys():
                left[index], right[index] = list(map(float, \
                                                horizontal[key].split(",")))
                upper[index] = left[index]
            elif key in vertical.keys():
                left[index], upper[index] = list(map(float, \
                                                vertical[key].split(",")))
                right[index] = left[index]
            else:
                pass
            
        # Next, add all of the defaults
        for index, key in enumerate(params.keys()):
            if not key in vertical.keys() and not key in horizontal.keys():
                if key in defaults.keys():
                    value = float(defaults[key])
                else: # not specified explicitly, so use default values
                    value = float(params[key])
                left[index] = value
                right[index] = value
                upper[index] = value
            else:
                pass

        for index in range(len(params.keys())):
            vertical_dr[index] = (upper[index]-left[index])/(self.height-1)

        return((left, right, vertical_dr))

    def __iter__(self):
        return(self)

    def __next__(self):
        if self.n < 0:
            pass
        else:
            increment_array(self.left, self.vertical_dr)
            increment_array(self.right, self.vertical_dr)

        self.n += 1
        
        if self.n >= self.height:
            raise StopIteration

        return((self.left, self.right))

class Line(object):
    def __init__(self, left=None, right=None, n_variables=0, n_points=100):
        self.left = left
        self.r = left
        self.right = right
        self.dr = (ctypes.c_double*n_variables)()
        for i in range(n_variables):
            self.dr[i] = (right[i]-left[i])/(n_points-1)
        self.n_points = n_points
        self.n = -1

    def __iter__(self):
        return(self)

    def __next__(self):
        if self.n < 0:
            pass
        else:
            increment_array(self.r, self.dr)

        self.n += 1
        
        if self.n >= self.n_points:
            raise StopIteration

        return((self.r))

class Options(object):
    def __init__(self, filename=None):
        self.filename = filename
        self.parser = configparser.SafeConfigParser()

        # Required for all runs
        self.system = None
        self.integrator = None
        self.t = None
        self.height = None
        self.width = None

        # optional in all runs
        self.restart = None

        # required for images
        self.rule = None
        
        # optional for images
        self.validator = None   
        self.extend_time = None

        # required for videos
        self.video = None
        self.write_every = None

        # placeholders for internal stuff
        self.n_variables = None
        self.points = None

        self.load()

    def get(self, section, key):
        try:
            return(self.parser.get(section, key))
        except:
            return(None)

    def getboolean(self, section, key):
        try:
            return(self.parser.getboolean(section, key))
        except:
            return(False)

    def getint(self, section, key):
        try:
            return(self.parser.getint(section, key))
        except:
            return(0)

    def get_section(self, section):
        result = dict()
        try:
            for key, value in self.parser.items(section):
                result[key] = value
        except:
            raise(\
                ValueError(\
                    "Section {0} required but not found.".\
                    format(section)))

        return(result)

    def validate_config(self, section, name, available, optional=False):
        # For a configuration option name with specified value chosen, check
        # the available options and return the correct one. If we can accept
        # a null result, return the default value, but otherwise throw an
        # error and let the user know something is wrong.
        chosen = self.get(section, name)
        result = None
        for elem in available:
            if elem.name == chosen:
                result = elem
                break
        if optional:
            return(result)
        else:
            if result == None:
                raise(ValueError("{0} {1} is not available.".\
                                 format(name.capitalize(), chosen)))
            else:
                return(result)

    def validate_size(self, section, key):
        size = self.getint(section, key)
        if size <= 0:
            raise(ValueError(\
                "{0} must be greater than zero. Tried: {1}".\
                format(key, size)))
        
        return(size)

    def load(self):
        """Load options to the grapher from an options object."""
        if os.path.isfile(self.filename):
            self.parser.read(self.filename)
        else:
            print("Could not open %s for reading." % self.filename)
            return(False)

        self.system = self.validate_config("config", "system", \
                                               systems.installed)

        if self.get("config", "integrator") == None:
            # The default integrator has the same name as the system
            self.integrator = self.validate_config("config", "system", \
                                                self.system.integrators)
        else:
            self.integrator = self.validate_config("config", "integrator", \
                                                self.system.integrators)
       
        try:
            # We need to check that the time increment will actually run in the
            # same direction as the upper limit.
            self.t = list(map(float, self.get("config", "t").split(",")))
            t, dt, t_limit = self.t

            if sign(t_limit-t) != sign(dt):
                raise(ValueError("Chosen time step will never complete."))
            else:
                pass
        except:
            raise(\
                ValueError(\
                    "Invalid time steps: {0}.".\
                    format(self.get("config", "t"))))

        self.height = self.validate_size("config", "height")
        self.width = self.validate_size("config", "width")
        
        self.restart = self.getboolean("config", "restart")
        
        video = self.get("config", "video")
        video_run = (video != None)
        if video_run:
            # We have found a video line, so parse it to make sure the variables
            # requested are actually available
            self.video = list()
            video = list(map(lambda x: x.strip(), \
                             video.split(",")))
            if video == []:
                raise(\
                    ValueError(\
                        "Must specify at least one variable to display."))

            keys = list(self.system.params.keys())
            for variable in video:
                if not variable in self.system.params.keys():
                    raise(\
                        ValueError(\
                            "Invalid video variable {0} found.".\
                            format(variable)))
                else:
                    self.video.append((variable, keys.index(variable)))

            self.write_every = self.getint("config", "write_every")
            if self.write_every <= 0:
                raise(ValueError("write_every must be a positive integer."))
        else:
            # doing an image run, go from there.
            self.rule = self.validate_config("config", "rule", \
                                             self.system.rules)
            self.validator = self.validate_config("config", "validator", \
                                    self.rule.validators, optional=True)
            self.extend_time = self.getboolean("config", "extend_time")

        self.n_variables = len(self.system.params.keys())                                            
        self.points = Points(self.get_section("horizontal"), \
                             self.get_section("vertical"), \
                             self.get_section("defaults"), \
                             self.system.params, \
                             self.height, \
                             self.width)
                  
if __name__ == "__main__":
    options = Options("test.inp")
