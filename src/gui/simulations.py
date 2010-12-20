import subprocess

class InputDeck(object):
    def __init__(self):
        self.params = dict()
        self.base_name = str()

    def __str__(self):
        result = str()
        for param in sorted(self.params.keys()):
            result += param
            result += " = "
            if type(self.params[param]) == type(list()):
                result += ",".join(map(str, self.params[param]))
            else:
                result += str(self.params[param])
            result += "\n"
        return(result)
