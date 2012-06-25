import ctypes
import os

def sign(x):
    return(x/abs(x))

def increment_array(array, dr):
    for index, elem in enumerate(dr):
        array[index] += elem

class Points(object):
    def __init__(self, horizontal=None, vertical=None, defaults=None, \
                 params=None, height=100, width=100):
        self.height = height
        self.width = width

        self.left, self.right, self.vertical_dr = \
                   self.from_config(horizontal, vertical, \
                                    defaults, params, height)
        self.n = -1
        self.n_variables = len(params.keys())

    def from_config(self, horizontal, vertical, defaults, params, size):
        """Fill the beginning and end of the line with the values from
variables, taken from the configuration file. For those which are not directly
specified, the values are taken from defaults. Anything not specified in the
input file gets taken from params, which are set externally."""
        n_variables = len(params.keys())
        left = (ctypes.c_double*n_variables)()
        right = (ctypes.c_double*n_variables)()
        upper = (ctypes.c_double*n_variables)()
        vertical_dr = (ctypes.c_double*n_variables)()

        for key in horizontal.keys():
            if key in vertical.keys():
                print("Cannot specify the same variable on both axes: {0}.".\
                      format(key))
            if key in defaults.keys():
                print(\
            "Cannot specify the same variable on an axis and a default: {0}.".\
            format(key))

        # First, build the values which actually vary. 
        for index, key in enumerate(params.keys()):
            if key in horizontal.keys():
                left[index], right[index] = list(map(float, \
                                                horizontal[key].split(",")))
                upper[index] = left[index]
            elif key in vertical.keys():
                left[index], upper[index] = list(map(float, \
                                                vertical[key].split(",")))
                right[index] = left[index]
            else:
                pass
            
        # Next, add all of the defaults
        for index, key in enumerate(params.keys()):
            if not key in vertical.keys() and not key in horizontal.keys():
                if key in defaults.keys():
                    value = float(defaults[key])
                else: # not specified explicitly, so use default values
                    value = float(params[key])
                left[index] = value
                right[index] = value
                upper[index] = value
            else:
                pass

        for index in range(len(params.keys())):
            vertical_dr[index] = (upper[index]-left[index])/(self.height-1)

        return((left, right, vertical_dr))

    def __iter__(self):
        return(self)

    def __next__(self):
        if self.n < 0:
            pass
        else:
            increment_array(self.left, self.vertical_dr)
            increment_array(self.right, self.vertical_dr)

        self.n += 1
        
        if self.n >= self.height:
            raise StopIteration

        return((self.left, self.right))

class Line(object):
    def __init__(self, left=None, right=None, n_variables=0, n_points=100):
        self.left = left
        self.r = left
        self.right = right
        self.dr = (ctypes.c_double*n_variables)()
        for i in range(n_variables):
            self.dr[i] = (right[i]-left[i])/(n_points-1)
        self.n_points = n_points
        self.n = -1

    def __iter__(self):
        return(self)

    def __next__(self):
        if self.n < 0:
            pass
        else:
            increment_array(self.r, self.dr)

        self.n += 1
        
        if self.n >= self.n_points:
            raise StopIteration

        return((self.r))

