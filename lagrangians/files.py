import bz2
import os

def compressed_file_exists(filename):
    return(os.path.isfile(filename) \
           or os.path.isfile("{0}.bz2".format(filename)))

def compress(filename):
    zip_filename = "{0}.bz2".format(filename)
    
    if os.path.isfile(zip_filename):
        pass
    else:
        with open(filename, "rb") as in_file:
            with bz2.BZ2File(zip_filename, "wb") as out_file:
                out_file.writelines(in_file)
                
        os.remove(filename)

def decompress(filename):
    zip_filename = "{0}.bz2".format(filename)

    if os.path.isfile(filename):
        pass
    else:
        with bz2.BZ2File(zip_filename, "rb") as in_file:
            with open(filename, "wb") as out_file:
                out_file.writelines(in_file)
                
        os.remove(zip_filename)

if __name__ == "__main__":
    filename = "/home/tsbischof/src/lagrangians/run/"\
               "prints/dick_blick/print/dangling_stick00000011.raw.999.1000"
    compress(filename)
    decompress(filename)
