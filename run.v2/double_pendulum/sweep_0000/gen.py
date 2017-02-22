#!/usr/bin/env python3

import numpy

input_file = """[config]
run_type = image

system = double_pendulum
t = 0, 100, 0.005

endpoint = lower_flip

width = 2500
height = 2500

[defaults]
l1 = 1
l2 = 1
m1 = {}
m2 = 1
g = 9.8
dphi1 = 0
dphi2 = 0

[horizontal]
phi1 = -6.28318530718, 6.28318530718

[vertical]
phi2 = -6.28318530718, 6.28318530718"""

for index, m in enumerate(numpy.arange(0.001, 10.00005, 0.001)):
    with open("{0:05d}.inp".format(index), "w") as stream_out:
        stream_out.write(input_file.format(m))


