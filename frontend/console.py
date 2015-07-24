#!/usr/bin/python
from ctypes import *
import readline
import code


ms_lib = CDLL("libmaplesyrup.so")


vars = globals().copy()
vars.update(locals())
shell = code.InteractiveConsole(vars)
shell.interact()
