import configparser
import os
import tempfile
from collections import OrderedDict

import systems

def sign(x):
    return(x/abs(x))

class Line(object):
    def __init__(self, variables=dict(), defaults=dict(), params=dict(),\
                 n_points=100):
        self.left, self.right = self.from_config(variables, defaults, params)
        self.n_points = n_points

        self.r = None
        self.dr = list(map(lambda x: (x[1]-x[0])/float(n_points-1), \
                        zip(self.left, self.right)))

    def from_config(self, variables, defaults, params):
        """Fill the beginning and end of the line with the values from
variables, taken from the configuration file. For those which are not directly
specified, the values are taken from defaults. Anything not specified in the
input file gets taken from params, which are set externally."""
        # To do: check that the two axes do not conflict; they should not
        # include the same variables
        left = list()
        right = list()
   
        for key in params.keys():
            if key in variables.keys():
                my_left, my_right = list(map(float, variables[key].split(",")))
            elif key in defaults.keys():
                value = float(defaults[key])
                my_left = value
                my_right = value
            else:
                value = float(params[key])
                my_left = value
                my_right = value
            left.append(my_left)
            right.append(my_right)

        return((left, right))

    def __iter__(self):
        return(self)

    def __next__(self):
        if self.r == None:
            self.point_number = 0
            self.r = self.left
        else:
            self.point_number += 1
            self.r = list(map(lambda x: x[0]+x[1], zip(self.r, self.dr)))
        
        if self.point_number >= self.n_points:
            raise StopIteration

        return(self.r)
        

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
        self.restart = bool()
        self.extend_time = bool()
        self.t = str()

class Installed(object):
    def __init__(self):
        self.system = str()
        self.integrator = None
        self.rule = None
        self.horizontal = None
        self.vertical = None

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

    def getbool(self, section, key):
        try:
            return(self.parser.getbool(section, key))
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
            
        self.options.config.t = self.get("config", "t")
        self.options.config.restart = self.getbool("config", "restart")
        self.options.config.extend_time = self.getbool("config", "extend_time")
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
            print("{0}: {1}.".format(name.capitalize(), chosen))
            
        return(result)

    def validate_size(self, dimension, size):
        if size <= 0:
            print("{0} must be greater than zero. Tried: {1}".format(\
                dimension, width))
        else:
            print("{0}: {1}.".format(dimension.capitalize(), size))

        return(size)
        
    def validate(self):
        # Go through all of the options and see that they make sense; things
        # should be installed, variables exist, etc.
        self.run.system = self.validate_config("system", \
                self.options.config.system, systems.installed)

        self.run.integrator = self.validate_config("integrator", \
                self.options.config.system, self.run.system.integrators)

        self.run.rule = self.validate_config("rule", \
                self.options.config.rule, self.run.system.rules)

        self.run.validator = self.validate_config("validator", \
                self.options.config.validator, self.run.rule.validators)

        self.run.restart = self.options.config.restart

        try:
            # We need to check that the time increment will actually run in the
            # same direction as the upper limit.
            self.run.t = list(map(float, self.options.config.t.split(",")))
            t, dt, t_limit = self.run.t

            if sign(t_limit-t) != sign(dt):
                raise(ValueError)
            else:
                print("Time steps: {0}.".format(self.run.t))
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
        self.run.horizontal = Line(self.options.horizontal, \
                                self.options.defaults, self.run.system.params,\
                                self.run.width)
        self.run.vertical = Line(self.options.vertical, \
                                self.options.defaults, self.run.system.params,\
                                self.run.height)

        # Now, everything should be ready to go. All values and functions
        # that are needed for the run are stored in self.run, and we can begin
        # doing the number-crunching.
                  
if __name__ == "__main__":
    options = Options("test.inp")
    options.load()
    
