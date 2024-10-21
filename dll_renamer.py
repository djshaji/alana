import os, glob

g = glob.glob ("libs/win32/*dll")

for l in g:
    dest = l.replace (".dll", ".so")
    print (f"{l} -> {dest}")
    os.rename (l, dest)
