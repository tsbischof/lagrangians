import os
import subprocess

def contains(L, items):
    return(any(map(lambda x: x in L, items)))

if __name__ == "__main__":
    srcs = list()
    dst_folder = "small"
    dsts = list()

    for root, dirs, files in os.walk("."):
        if contains(root, ["dangling_stick", "double_pendulum",
                           "double_spring", "double_well",
                           "harmonic_oscillator", "springy_pendulum"]) \
            and not contains(root, ["sweep", "video", "like",
                                    "small", "prints"]):
                for filename in files:
                    if filename.endswith(".png"):
                        srcs.append((root, filename))

    for root, filename in srcs:
        src = os.path.join(root, filename)
        dst = os.path.join(dst_folder, filename)
        if not os.path.isfile(dst):
            print("{0} -----> {1}".format(src, dst))
            cmd = ["convert", src, "-resize", "1000x", dst]
            subprocess.Popen(cmd).wait()
                        
