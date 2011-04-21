import math
import re

input_file = """
system = dangling_stick
rule = first_flip

t = 0, 0.01, 300
phi2 = 1,2000, 2
r = 1.5000,1000, 2.0000
dr = 2
dphi2 = %f
m1 = 1
m2 = 1
plot = r, phi2
"""

val = -3
dval = 0.01
limit = -1

while val <= limit:
    f = open("dangling_stick_dphi2_%s.inp" % (re.sub("\.", "_", "%.3f" % val)), "w")
    f.write(input_file % (val))
    f.close()
    val += dval
