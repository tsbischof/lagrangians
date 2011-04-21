import math
import re

input_file = """system = double_spring
rule = lower_flip
validate = energy

t = 0,0.01,200
phi1 = -3.1415926, 2000, 3.1415926
phi2 = -3.1415926, 2000, 3.1415926
m1 = 1
m2 = 1
dr1 = %f
dr2 = -%f
plot = phi1,phi2
"""

val = 1
dval = 0.05
limit = 3

while val <= limit:
    f = open("double_spring_dr1_%s.inp" % (re.sub("\.", "_", "%.3f" % val)), "w")
    f.write(input_file % (val, val))
    f.close()
    val += dval
