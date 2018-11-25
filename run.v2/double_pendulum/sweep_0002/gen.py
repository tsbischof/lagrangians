#!/usr/bin/env python3

import numpy

input_file = """[config]
run_type = image

system = double_pendulum
t = 0, 100, 0.005

endpoint = lower_flip

width = 2500
height = 1250

[defaults]
l1 = 1
l2 = 1
m1 = {}
m2 = {}
g = 9.8
dphi1 = 0
dphi2 = 0

[horizontal]
phi1 = -6.28318530718, 6.28318530718

[vertical]
phi2 = 0, 6.28318530718"""

p = numpy.linspace(0, 2*numpy.pi, 100)[:-1]
m1s = 1.25 + 0.75*numpy.sin(p)
m2s = 1.25 + 0.75*numpy.cos(p)

for index, (m1, m2) in enumerate(zip(m1s, m2s)):
    with open("{0:05d}.inp".format(index), "w") as stream_out:
        stream_out.write(input_file.format(m1, m2))


