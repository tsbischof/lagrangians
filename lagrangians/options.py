import os
import configparser
from collections import OrderedDict

from lagrangians import systems, modes, points

def sign(x):
    if x < 0:
        return(-1)
    else:
        return(1)

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

        self.read()

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

    def read(self):
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
       
        # We need to check that the time increment will actually run in the
        # same direction as the upper limit.
        self.t = list(map(float, self.get("config", "t").split(",")))
        t, dt, t_limit = self.t

        if sign(t_limit-t) != sign(dt):
            raise(ValueError("Chosen time step will never complete."))
        else:
            pass

        self.height = self.validate_size("config", "height")
        self.width = self.validate_size("config", "width")
        
        self.restart = self.getboolean("config", "restart")
        
        video = self.get("config", "video")
        video_run = (video != None)
        if video_run:
            # We have found a video line, so parse it to make sure the variables
            # requested are actually available
            self.mode = modes.MODE_VIDEO
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
                    raise(
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
            self.mode = modes.MODE_IMAGE
            self.rule = self.validate_config("config", "rule", \
                                             self.system.rules)
            self.validator = self.validate_config("config", "validator", \
                                    self.rule.validators, optional=True)
            self.extend_time = self.getboolean("config", "extend_time")

        self.n_variables = len(self.system.params.keys())                                            
        self.rows = points.Points(
            self.get_section("horizontal"),
            self.get_section("vertical"),
            self.get_section("defaults"),
            self.system.params,
            self.height,
            self.width)

    def to_file(self):
        pass
                  
if __name__ == "__main__":
    options = Options("test.inp")
