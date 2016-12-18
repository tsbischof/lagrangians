import math
import random

input_file = """[config]
system = springy_pendulum
rule = lower_flip

t = 0, 0.005, 100

width = 500
height = 500

[horizontal]
phi2 = %f, %f

[vertical]
dr = -2, 2

""" % (-math.pi, math.pi) + \
"""[defaults]
r = %f
phi1 = %f
dphi1 = %f
dphi2 = %f
m1 = 1
m2 = 1
r_0 = 1
k = 1
l = 1
g = 9.8
""" 

for i in range(50, 500):
    vals = (2*random.random(), 2*random.random()-1, 2*random.random()-1, 2*random.random()-1)
    f = open("springy_pendulum_%010d.inp" % i, "w")
    f.write(input_file % vals)
    f.close()
