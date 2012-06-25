#!/usr/bin/env python3

import os
import sys
import configparser
import subprocess
import tempfile
import math
import struct

def strip_all(x):
    return(list(map(lambda y: y.strip(), x)))

def read_config(filename):
    config_file = tempfile.TemporaryFile("w+t")
    config_file.write('[config]\n')
    f = open(filename, "r")
    for line in f:
        config_file.write(line)
    f.close()
    config_file.seek(0)

    parser = configparser.SafeConfigParser()
    parser.readfp(config_file)
    return(parser)

def optional(new_parser, old_parser, section, key):
    try:
        new_parser.set(section, key, old_parser.get("config", key))
    except:
        pass

def required(new_parser, old_parser, section, key):
    new_parser.set(section, key,  old_parser.get("config", key))

def translate(parser, filename):
    out_parser = configparser.SafeConfigParser()
    out_parser.add_section("config")
    out_parser.add_section("defaults")
    out_parser.add_section("horizontal")
    out_parser.add_section("vertical")

    seen_keys = ["system", "rule", "validate", "integrator", "rule", "t", "plot", "restart"]
    required(out_parser, parser, "config", "system")
    required(out_parser, parser, "config", "rule")
    optional(out_parser, parser, "config", "validate") 
    optional(out_parser, parser, "config", "integrator") 
    optional(out_parser, parser, "config", "rule") 
    optional(out_parser, parser, "config", "restart")

    t = strip_all(parser.get("config", "t").split(","))
    out_parser.set("config", "t", ", ".join(t))

    plot = strip_all(parser.get("config", "plot").split(","))
    seen_keys.extend(plot)
    horizontal = strip_all(parser.get("config", plot[1]).strip().split(","))
    vertical = strip_all(parser.get("config", plot[0]).strip().split(","))
    out_parser.set("config", "height", vertical[1])
    out_parser.set("config", "width", horizontal[1])

    out_parser.set("horizontal", plot[0], ", ".join([horizontal[0], horizontal[2]]))
    out_parser.set("vertical", plot[1], ", ".join([vertical[0], vertical[2]]))

    for key, value in parser.items("config"):
        if not key in seen_keys:
            out_parser.set("defaults", key, value)
 

    out_file = open(filename + ".new", "w")
    out_parser.write(out_file)
    out_file.close()

def find_files(folder):
    for root, dirs, files in os.walk(folder):
        for filename in filter(lambda x: x.endswith(".inp"), files):
            yield(filename)

if __name__ == "__main__":
    filenames = sys.argv[1:]
    if len(filenames) == 0:
        filenames = find_files(os.getcwd())
        
    for filename in filenames:
        parser = read_config(filename)
        translate(parser, filename)
