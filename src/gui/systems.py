import re

class System(object):
    def __init__(self, name):
        self.name = name
        self.params = list()
        self.defaults = list()
        self.rules = list()
        self.validators = list()
        
system_names = ["dangling_stick", "double_pendulum", "double_spring", \
           "springy_pendulum", "harmonic_oscillator", "trig", "double_well"]

systems = dict()

for system in system_names:
    f = open("../systems/%s.c" % system)
    data = f.read()
    f.close()
    myvars = re.search("enum {(?P<vars>.+)};", data)
    myvars = list(map(lambda x: x.lower(),
                      re.sub("[ ]", "", myvars.group("vars")).split(",")))

    mydefaults = re.search(\
        "variable_defaults\[.+\][\W]+=[\W]+{(?P<defaults>[^;]+)};", data)
    mydefaults = list(map(float, \
                          mydefaults.group("defaults").split(",")))

    myrules = re.findall("functions.rule_names\[.+\][\W]+=[\W]+\"([^;]+)\";",
                        data)
    myvalidators = re.findall(\
        "functions.validate_names\[.+\] = \"([^;]+)\";", data)
    
    systems[system] = System(system)
    systems[system].params = myvars
    systems[system].defaults = mydefaults
    systems[system].rules = myrules
    systems[system].validators = myvalidators

