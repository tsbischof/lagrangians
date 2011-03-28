import ctypes

lagrangians = ctypes.cdll.LoadLibrary("./liblagrangians.so")
colormapper = ctypes.cdll.LoadLibrary("./libcolormap.so")
