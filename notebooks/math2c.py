import sys
import re

# Does a quick job of converting the output of a Mathematica call to
# CForm[EquationsOfMotion[#]] to real C syntax. This is of course limited
# to certain C expressions, but it should be possible to extend it a bit.
#
# To do: make this work for T, U. 

file = "".join(map(lambda x: x.strip(), sys.stdin.readlines()))

reserved = ["cos", "sin", "pow"]
subs = [["Cos", "cos"], ["Sin", "sin"], ["Power", "pow"], ["==", "="], \
        [r"\(t\)", ""], [" ", ""], ["\\\\", ""]]

equation_type = True

for in_word, out_word in subs:
    file = re.sub(in_word, out_word, file)


file_tmp = file
for m in re.finditer(r"\[(?P<var>[^]]+)]", file):
    file_tmp = re.sub(r"\[%s]" % m.group("var"), m.group("var").lower(), file_tmp)
file = file_tmp
file_tmp = file
for m in re.finditer("Subscript\((?P<var>[^,]+),(?P<num>[0-9]+)\)", file):
    file_tmp = re.sub("Subscript\(%s,%s\)" % (m.group("var"), m.group("num")), \
                      "%s%s" % (m.group("var"), m.group("num")), file_tmp)
file = file_tmp
file_tmp = file
for m in re.finditer("Derivative\((?P<order>[0-9]+)\)\((?P<var>[^)]+)\)",
                     file):
    file_tmp = re.sub("Derivative\(%s\)\(%s\)" % (m.group("order"), \
                                                  m.group("var")), \
                      "d%s" % m.group("var").lower(), file_tmp)
file = file_tmp
result = list()
for line in map(lambda x: re.sub(",$", "",x ),
                filter(lambda x: x, file.split("Rule"))):
    m = re.search("^\((?P<var>[^,]+),(?P<right>.+)\)$", line)
    if m:
        result.append("drdt[%s] = %s" % (m.group("var"), m.group("right")))
    else:
        equation_type = False # we have a general expression, not a rule
        result.append(line)
file = file_tmp

separators = "()[]-+*/=,. "
digits = "1234567890"
my_vars = set()
final_result = list()
for line in result:
    my_line = list()
    current = str()
    for c in line:
        if not c in separators:
            current += c
        else:
            my_line.append(current)
            my_line.append(c)
            current = ""

    my_line.append(current)

    final_line = str()
    left = True
    for term in my_line:
        if term in separators or term in reserved or term == "drdt" \
           or term in digits:
            if term == "=":
                left = False
            final_line += term
        else:
            my_vars.add(term.upper())
            if left and equation_type:
                final_line += term.upper()
            else:
                final_line += "r[%s]" % term.upper()
    final_result.append(final_line)
    
for line in final_result:
    print("\t%s;" % line)

if equation_type:
    for var in my_vars:
        print("\tdrdt[%s] = 0;" % var)
