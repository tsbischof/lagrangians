import configparser
import ctypes
import os
import tempfile
from collections import OrderedDict

import systems

def list_to_array(array, L):
    for i in range(len(L)):
        result[i] = L[i]
    return(result)

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
                 params=None, size=100):
        self.n_points = size
        self.left, self.right, self.dr = self.from_config(horizontal, vertical, \
                                                 defaults, params, size)
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
        dr = (ctypes.c_double*n_variables)()

        for key in horizontal.keys():
            if key in vertical.keys():
                print("Cannot specify the same variable on both axes: {0}.".\
                      format(key))
            if key in defaults.keys():
                print(\
  "Cannot specify the same variable on an axis and a default: {0}.".format(key))

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
            dr[index] = (upper[index]-left[index])/(self.n_points-1)

        return((left, right, dr))

    def __iter__(self):
        return(self)

    def __next__(self):
        if self.n < 0:
            pass
        else:
            increment_array(self.left, self.dr)
            increment_array(self.right, self.dr)

        self.n += 1
        
        if self.n >= self.n_points:
            raise StopIteration

        return((self.n, self.left, self.right))

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

        return((self.n, self.r))

class Parsed(object):
    def __init__(self):
        self.config = Config()
        self.defaults = dict()
        self.horizontal = dict()
        self.vertical = dict()

class Config(object):
    def __init__(self):
        self.system = str()
        self.integrator = str()
        self.validator = str()
        self.rule = str()
        self.video = str()
        self.write_every = str()
        self.restart = bool()
        self.extend_time = bool()
        self.t = str()

class Installed(object):
    def __init__(self):
        self.system = str()
        self.integrator = None
        self.rule = None
        self.points = None
        self.video = list()
        self.write_every = None

class Options(object):
    def __init__(self, filename=None):
        self.filename = filename
        self.parser = configparser.SafeConfigParser()
        self.options = Parsed()
        self.run = Installed()

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
            pass

        return(result)

    def load(self):
        """Load options to the grapher from an options object."""
        if os.path.isfile(self.filename):
            self.parser.read(self.filename)
        else:
            print("Could not open %s for reading." % self.filename)
            return(False)

        self.options.config.system = self.get("config", "system")
        self.options.config.rule = self.get("config", "rule")
        self.options.config.validator = self.get("config", "validator")

        # If no integrator is specified, use the default one
        self.options.config.integrator = self.get("config", "integrator")
        if self.options.config.integrator == None:
            self.options.config.integrator = self.options.config.system

        # For videos, we need to know how often we will be printing (integer)
        self.options.config.video = self.get("config", "video")
        self.options.config.write_every = self.get("config", "write_every")
            
        self.options.config.t = self.get("config", "t")
        self.options.config.restart = self.getboolean("config", "restart")
        self.options.config.extend_time = self.getboolean("config", "extend_time")
        self.options.config.width = self.getint("config", "width")
        self.options.config.height = self.getint("config", "height")

        self.options.defaults = self.get_section("defaults")
        self.options.horizontal = self.get_section("horizontal")
        self.options.vertical = self.get_section("vertical")

        self.validate()

    def validate_config(self, name, chosen, available):
        # For a configuration option name with specified value chosen, check
        # the available options and return the correct one. If we can accept
        # a null result, return the default value, but otherwise throw an
        # error and let the user know something is wrong.
        result = None
        for elem in available:
            if elem.name == chosen:
                result = elem
                break
        if result == None:
            print("{0} {1} is not available.".format(name.capitalize(), chosen))
        else:
            pass
##            print("{0}: {1}.".format(name.capitalize(), chosen))
            
        return(result)

    def validate_size(self, dimension, size):
        if size <= 0:
            print("{0} must be greater than zero. Tried: {1}".format(\
                dimension, width))
        else:
            pass
##            print("{0}: {1}.".format(dimension.capitalize(), size))

        return(size)
        
    def validate(self):
        # Go through all of the options and see that they make sense; things
        # should be installed, variables exist, etc.
        self.run.system = self.validate_config("system", \
                self.options.config.system, systems.installed)

        self.run.integrator = self.validate_config("integrator", \
                self.options.config.system, self.run.system.integrators)

        if self.options.config.video == None:
            self.run.rule = self.validate_config("rule", \
                    self.options.config.rule, self.run.system.rules)
            self.run.validator = self.validate_config("validator", \
                    self.options.config.validator, self.run.rule.validators)
        else:
            video = list(map(lambda x: x.strip(), \
                             self.options.config.video.split(",")))
            if video == []:
                print("Must specify at least one variable to display.")

            keys = list(self.run.system.params.keys())
            for variable in video:
                if not variable in self.run.system.params.keys():
                    print("Invalid video variable {0} found.".format(variable))
                else:
                    self.run.video.append((variable, keys.index(variable)))

            self.run.write_every = int(self.options.config.write_every)
            if self.run.write_every <= 0:
                print("write_every must be a positive integer.")

        self.run.n_variables = len(self.run.system.params.keys())
        self.run.restart = self.options.config.restart
##        print("Restart: {0}".format(self.run.restart))

        try:
            # We need to check that the time increment will actually run in the
            # same direction as the upper limit.
            self.run.t = list(map(float, self.options.config.t.split(",")))
            t, dt, t_limit = self.run.t

            if sign(t_limit-t) != sign(dt):
                raise(ValueError)
            else:
                pass
##                print("Time steps: {0}.".format(self.run.t))
        except:
            print("Invalid time steps: {0}.".format(self.options.config.t))

        self.run.width = self.validate_size("width", \
                                            self.options.config.width)
        self.run.height = self.validate_size("height", \
                                             self.options.config.height)

        # We have now validated the important options that are required for
        # any run. The rest are sort of softer: variables which are not
        # specified explicitly will still have some default values associated
        # with them, so we should silently use the default values. However,
        # if a value is specified but not used (e.g. dphi12 when
        # dphi1 was meant), we should at least notify the user.
        self.run.points = Points(self.options.horizontal, \
                                 self.options.vertical, \
                                 self.options.defaults, \
                                 self.run.system.params, \
                                 self.run.height)

        # Now, everything should be ready to go. All values and functions
        # that are needed for the run are stored in self.run, and we can begin
        # doing the number-crunching.
                  
if __name__ == "__main__":
    options = Options("test.inp")
    options.load()
    
