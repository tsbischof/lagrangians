input_file = """system = double_pendulum
rule = first_flip
validator = energy

t = 0,0.01,100
phi1 = -3.1415926,0.0062831,3.1415926
phi2 = -3.1415926,0.0062831,3.1415926
dphi1 = %f
dphi2 = %f
plot = phi1,phi2
"""
input_file = """system = double_pendulum
rule = first_flip
validator = energy

t = 0,0.01,100
dphi1 = -5,0.01,5
dphi2 = -5,0.01,5
phi1 = %f
phi2 = %f
plot = dphi1,dphi2
"""


dphi1 = -3
dphi2_init = -3
ddphi1 = 0.1
ddphi2 = 0.1

dphi1_limit = 3.01
dphi2_limit = 3.01

n = 100000
while dphi1 <= dphi1_limit:
    dphi2 = dphi2_init
    while dphi2 <= dphi2_limit:
        f = open("dp%08d.inp" % n, "w")
        f.write(input_file % (dphi1, dphi2))
        f.close()
        n += 1
        dphi2 += ddphi2
    dphi1 += ddphi1
