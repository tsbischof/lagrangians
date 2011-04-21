import itertools

input_file = """system = trig
integrator = full 
rule = first_turnaround

t = 0,0.01,100
x = -5,1000,5
v = -5,1000,5
k1 = %f
k2 = %f
k3 = %f
k4 = %f
k5 = %f
k6 = %f
plot = x, v
"""

def drange(start, step, stop):
    r = start
    while r <= stop:
        yield r
        r += step      

n = 0

for ks in itertools.product(drange(0,0.1,1), drange(0,0.1,1), \
                             drange(0,1,2), drange(0,1,2), \
                             drange(0,1,2), drange(0,1,2)):
    f = open("trig%08d.inp" % n, "w")
    f.write(input_file % ks)
    f.close()
    n += 1
