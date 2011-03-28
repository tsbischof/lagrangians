from collections import OrderedDict

from c_libraries import lagrangians

class System(object):
    def __init__(self, name=None, params=None, rules=None, integrators=None, \
                 comment=""):
        self.name = name
        self.params = params
        self.rules = rules
        self.integrators = integrators
        self.comment = comment

class Integrator(object):
    def __init__(self, name=None, function=None):
        self.name = name
        self.function = function

class Rule(object):
    def __init__(self, name=str(), function=None, validators=None):
        self.name = name
        self.function = function
        self.validators = validators

        if self.validators == None:
            self.validators = [Validator(None, lagrangians.validate_all)]
        else:
            self.validators.append(Validator(None, lagrangians.validate_all))

class Validator(object):
    def __init__(self, name=None, function=None):
        self.name = name
        self.function = function

# A system requires the following specifications:
# name: the default name for functions. All functions related to this system
#       should have this as a prefix, for clarity
# options: a dictionary containing the names and default values for all
#          variables known to the system, in order
# rules: a list of the rules for the system, which determine whether the
#        simulation is complete. Also includes validators, which determine
#        whether it is even possible for the given rule to be satisfied
# integrators: all methods for integrating the equatiions of motion for the
#              system

# One-dimensional walkers, these are the fastest to run and are mainly used
# to test the system.
driven_damped_pendulum = System("driven_damped_pendulum", \
    OrderedDict({"phi": 0, "dphi": 0, "m": 1, "b": 0, "g": 9.8, "l": 1,\
                 "a": 1, "omega": 1, "delta": 1}),\
    [Rule("first_flip", lagrangians.driven_damped_pendulum_first_flip)], \
    [Integrator("sinusoid", \
                lagrangians.driven_damped_pendulum_sinusoid_integrate),\
     Integrator("square", \
                lagrangians.driven_damped_pendulum_square_integrate)], \
    """Simulates the motion of a damped pendulum, with a driving force
set by the choice of integrator.
Descriptions of parameters:
phi: angle of the pendulum
m: mass at the end of the pendulum
b: damping coefficient
g: gravitational constant
l: length of the pendulum
a: driving amplitude
omega: driving angular frequency
delta: driving phase
A `d' in front of the variable indicates a time derivative.
""")


double_well = System("double_well", \
    OrderedDict([("x", 0), ("v", 0), ("m", 1), ("k1", 1), ("k2", 1)]), \
    [Rule("first_turnaround", lagrangians.double_well_first_turnaround),\
     Rule("speed", lagrangians.double_well_speed)], \
    [Integrator("double_well", lagrangians.double_well_integrate)])

harmonic_oscillator = System("harmonic_oscillator", \
    OrderedDict([("x", 0), ("v", 0), ("m", 1), ("k", 1)]), \
    [Rule("first_turnaround", lagrangians.harmonic_oscillator_first_turnaround), \
     Rule("speed", lagrangians.harmonic_oscillator_speed)],\
    [Integrator("harmonic_oscillator", \
                lagrangians.harmonic_oscillator_integrate)])

# A second pendulum makes things interesting
double_pendulum = System("double_pendulum", \
    OrderedDict({"phi1": 0, "dphi1": 0, "phi2": 0, "dphi2": 0, "m1": 1, \
                 "m2": 1, "l1": 1, "l2": 1, "g": 9.8}), \
    [Rule("lower_flip", lagrangians.double_pendulum_lower_flip, \
      [Validator("energy", lagrangians.double_pendulum_lower_flip_energy)])],\
    [Integrator("double_pendulum", lagrangians.double_pendulum_integrate)], \
    """Simulates the motion of two pendulums, attached to each other. The first
pendulum is attached to a fixed point, has a mass m1 attached to its lower
end, and has length l1. The second pendulum is fixed to the bottom of the
first, and has mass m2 and length l2. Phi and omega describe the angle and
angular velocity of the two angles.""")

# Throwing in a spring cranks up the insanity
dangling_stick = System("dangling_stick", \
    OrderedDict({"r": 1, "dr": 0, "phi1": 0, "dphi1": 0, \
                 "phi2": 0, "dphi2": 0, "m1": 1, "m2": 1, \
                 "r0": 1, "k": 1, "l": 1, "g": 9.8}), \
    [Rule("lower_flip", lagrangians.dangling_stick_lower_flip, \
      [Validator("energy", lagrangians.dangling_stick_lower_flip_energy)])],\
    [Integrator("dangling_stick", lagrangians.dangling_stick_integrate)], \
    """Simulates the motion of a massless stick with a mass on either end,
attached to a spring. Descriptions of parameters:
r: length of the spring
phi1: angle of the spring, relative to vertical.
phi2: angle of the stick, relative to vertical.
m1: mass of the mass connected to the spring
m2: mass of the mass dangling from the stick
k: spring constant for the spring
l: length of the stick
A `d' in front of the variable indicates a time derivative.""")

double_spring = System("double_spring",
    OrderedDict({"phi1": 0, "dphi1": 0, "phi2": 0, "dphi2": 0, \
                 "m1": 1, "m2": 1, \
                 "k1": 1, "r1": 1, "dr1": 1, "r1_0": 1, \
                 "k2": 1, "r2": 1, "dr2": 1, "r2_0": 1, \
                 "g": 9.8}),\
    [Rule("lower_flip", lagrangians.double_spring_lower_flip, \
          [Validator("energy", lagrangians.double_spring_lower_flip_energy)]), \
     Rule("upper_flip", lagrangians.double_spring_upper_flip, \
          [Validator("energy", lagrangians.double_spring_upper_flip_energy)])],\
    [Integrator("double_spring", lagrangians.double_spring_integrate)], \
    """Simulates the motion of two springs, in the same configuration as the
double pendulum.""")

platform_with_springs = System("platform_with_springs", \
    OrderedDict({"r1": 1, "dr1": 0, "r1_0": 1, "k1": 1, \
                 "m1": 1, "phi1": 0, "dphi1": 0, \
                 "r2": 1, "dr2": 0, "r2_0": 1, "k2": 1, \
                 "m2": 1, "phi2": 0, "dphi2": 0, \
                 "b": 4, "h": 1, "x0": 0, "y0": 0, \
                 "m3": 1, "phi3": 0, "dphi3": 0, \
                 "g": 9.8}), \
    [Rule("left_flip", lagrangians.platform_with_springs_left_flip), \
     Rule("right_flip", lagrangians.platform_with_springs_right_flip), \
     Rule("platform_flip", lagrangians.platform_with_springs_platform_flip)],\
    [Integrator("platform_with_springs", \
                lagrangians.platform_with_springs_integrate)], \
   """Simulates the motion of a (2D) block with length and height, rotating about
a fixed axis perpendicular to the plane of the block. At either end is a
spring with a mass attached.
r1: length of the left spring
r10: natural length of the left spring
k1: spring constant of the left spring
m1: mass dangling from the left spring
phi1: angle of the left spring, relative to vertical
r2, r20, k2, m2, phi2: ditto
b, h: base and height of the block
x0, y0: the origin about which the system rotates
m3: mass of the block
phi3: angle of the block, relative to vertical
 */
""")

springy_pendulum = System("springy_pendulum", \
    OrderedDict({"r": 1, "dr": 0, "phi1": 0, "dphi1": 0, \
                 "phi2": 0, "dphi2": 0, "m1": 1, "m2": 1, \
                 "r0": 1, "k": 1, "l": 1, "g": 9.8}),\
    [Rule("lower_flip", lagrangians.springy_pendulum_lower_flip, \
        [Validator("energy", lagrangians.springy_pendulum_lower_flip_energy)]),\
     Rule("upper_flip", lagrangians.springy_pendulum_upper_flip, \
        [Validator("energy", lagrangians.springy_pendulum_upper_flip_energy)])],\
    [Integrator("springy_pendulum", lagrangians.springy_pendulum_integrate)], \
    """Simulates the motion of a pendlum with a mass on a spring attached. The
pendulum arm has no mass, but a mass is attached to the end.
r: length of the spring
phi1: angle of the pendulum, relative to vertical.
phi2: angle of the spring, relative to vertical.
m1: mass of the mass connected to the pendulum
m2: mass of the mass connected to the spring
k: spring constant for the spring
l: length of the stick
A `d' in front of the variable indicates a time derivative.""")

trig = System("trig", \
    OrderedDict({"x": 0, "v": 0, "m": 1, \
                 "k1": 4, "k2": 4, "k3": 0.01, "k4": 0.01, \
                 "k5": 0.01, "k6": 0.01}),\
    [Rule("first_turnaround", lagrangians.trig_first_turnaround), \
     Rule("speed", lagrangians.trig_speed)], \
    [Integrator("trig", lagrangians.trig_integrate)])

installed = [dangling_stick, double_pendulum, double_spring, \
           double_well, driven_damped_pendulum, harmonic_oscillator, \
           platform_with_springs, springy_pendulum, trig]

