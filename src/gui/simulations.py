import subprocess

class Input_Deck(object):
    def __init__(self):
        self.params = dict()
        self.base_name = str()

    def __str__(self):
        result = str()
        for param in sorted(self.params.keys()):
            result += param
            result += " = "
            result += ",".join(map(str, self.params[param]))
            result += "\n"
        return(result)

    def set_name(self, base_name):
        self.base_name = base_name
        self.input_name = base_name + ".inp"
        self.raw_name = base_name + ".raw"
        self.ppm_name = base_name + ".ppm"

    def to_file(self):
        if not self.base_name:
            print("No filename set. Use set_name().")
            return()
        try:
            f = open(self.filename, "w")
            f.write(self.__str__())
            f.close()
        except:
            print("Error while writing to %s." % self.input_name)

    def do_simulation(self, log):
        if not self.base_name:
            print("No filename set. Use set_name().")
            return()
        return(subprocess.Popen(["lagrangians", self.input_name], stdout=log))

