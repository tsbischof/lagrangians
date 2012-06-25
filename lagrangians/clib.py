import ctypes
import os

lib_dir = "/home/tsbischof/lib"

lagrangians = ctypes.cdll.LoadLibrary(os.path.join(
    lib_dir, "liblagrangians.so"))
colormapper = ctypes.cdll.LoadLibrary(os.path.join(
    lib_dir, "libcolormap.so"))
