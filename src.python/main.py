import os
import sys

import parse
import grapher

def main(argv):
    for filename in sys.argv[1:]:
        grapher = grapher.Grapher(filename)
        grapher.run()
        grapher.to_ppm()
        grapher.to_png()

if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Usage: lagrangians <input_files>")
    else:
        main(sys.argv)
