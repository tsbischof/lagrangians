input_file = """integrator = double_pendulum
rule = first_flip
validator = energy_double_pendulum

t = 0,0.01,100
phi1 = -3.14,0.0314,3.14
phi2 = -3.14,0.0314,3.14
omega1 = %f
omega2 = %f
plot = phi1,phi2
"""

omega1 = -3
omega2 = -3

domega1 = 0.01
domega2 = 0.01

omega1_limit = 3
omega2_limit = 3

n = 0
while omega1 < omega1_limit:
    while omega2 < omega2_limit:
        f = open("dp%08d.inp" % n, "w")
        f.write(input_file % (omega1, omega2))
        f.close()
        n += 1
        omega1 += domega1
        omega2 += domega2
