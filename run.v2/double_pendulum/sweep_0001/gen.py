#!/usr/bin/env python3

import numpy

input_file = """[config]
run_type = image

system = double_pendulum
t = 0, 100, 0.005

endpoint = lower_flip

width = 250
height = 500

[defaults]
l1 = 1
l2 = 1
m1 = 1
m2 = {}
g = 9.8
dphi1 = 0
dphi2 = 0

[vertical]
phi1 = -6.28318530718, 6.28318530718

[horizontal]
phi2 = -6.28318530718, 0"""

for index, m in enumerate(numpy.arange(0.1, 10.01, 0.1)):
    with open("{0:05d}.inp".format(index), "w") as stream_out:
        stream_out.write(input_file.format(m))


