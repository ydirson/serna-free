#!/usr/bin/python

import sys, re

if 2 > len(sys.argv):
    sys.exit(-1)

repldir = sys.argv[1].replace('\\', '/').rstrip('/')
inc_re = re.compile(r'^\s*#\s*include\s+"(\.\./)+(?P<incname>[^"]+)".*$')

for fname in iter(sys.stdin):
    fname = fname.strip()
    newcontents = ""
    modified = False
    for line in file(fname, "r"):
        mobj = inc_re.match(line)
        if mobj:
            line = '#include "%s/%s"\n' % (repldir, mobj.group("incname"))
            modified = True
        newcontents += line
    if modified:
        file(fname, "w").write(newcontents)
