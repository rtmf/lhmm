#!/usr/bin/python
from os import *
from sys import *
for each in ["build/" + s.split(":")[0][4:].replace(".cpp",".o").replace(".h",".pch") + " : src/" + s.split(":")[1][len('#include "'):-1] for s in popen('grep include\\ \\" src/*', 'r').read().strip().split("\n")]:
    if each.find("oglconsole") == -1:
        print each
