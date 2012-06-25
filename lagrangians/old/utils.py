import bz2
import os
  
def compress(filename):
    with open(filename, "rb") as in_file:
        with bz2.BZ2File(filename + ".bz2", "wb") as out_file:
            out_file.writelines(in_file)
    os.remove(filename)

def decompress(filename):
    with bz2.BZ2File(filename, "rb") as in_file:
        with open(filename[:-4], "wb") as out_file:
            out_file.writelines(in_file)
    os.remove(filename)
