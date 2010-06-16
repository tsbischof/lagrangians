input_file = """system = double_pendulum
rule = first_flip
validator = energy

t = 0,0.01,100
phi1 = -3.14159,0.01256,3.14159
phi2 = -3.14159,0.01256,3.14159
omega1 = %f
omega2 = %f
plot = phi1,phi2
"""

omega1 = -3
omega2_init = -3

domega1 = 0.1
domega2 = 0.1

omega1_limit = 3.01
omega2_limit = 3.01

n = 0
while omega1 <= omega1_limit:
    omega2 = omega2_init
    while omega2 <= omega2_limit:
        f = open("dp%08d.inp" % n, "w")
        f.write(input_file % (omega1, omega2))
        f.close()
        n += 1
        omega2 += domega2
    omega1 += domega1
