#!/usr/bin/python

import os, sys, getopt, re

opts, args = getopt.getopt(sys.argv[1:], "x:o:")

excludes = []

def excluded(path):
    for r in excludes:
        if r.match(path):
            return True
    return False

outFile = ''

for o, a in opts:
    if '-x' == o:
        excludes.append(re.compile(a.strip()))
    elif '-o' == o:
        outFile = a

cwd = os.getcwd()
flist = []

for path in args:
    try:
        os.chdir(path)
    except:
        continue
    for top, dirs, files in os.walk('.'):
        for fpath in files:
            if excluded(fpath):
                continue
            fpath = os.path.join(top, fpath)
            if fpath.startswith('./') or fpath.startswith('.\\'):
                fpath = fpath[2:]
            flist.append("<file>%s</file>" % fpath)
    
if flist:
    outString = '\n'.join(flist)
    if outFile:
        open(outFile, "w+").write(outString + "\n")
    else:
        print outString

