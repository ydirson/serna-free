import os, sys, getopt, tempfile

opts, args = getopt.getopt(sys.argv[1:], "p:r:o:s")

pattern = replFile = outFile = ''
strip = False

for o, a in opts:
    if '-p' == o:
        pattern = a
    elif '-r' == o:
        replFile = a
    elif '-o' == o:
        outFile = a
    elif '-s' == o:
        strip = True

if args and pattern and replFile and os.path.isfile(replFile):
    subst = open(replFile, "r").read()
    if strip:
        subst = subst.strip()
    fpath = args[0]
    if os.path.isfile(fpath):
        fString = open(fpath, "r").read()
        fString = fString.replace(pattern, subst)
        if outFile:
            open(outFile, "w+").write(fString)
        else:
            h, tmpFile = tempfile.mkstemp('', 'tmp', os.path.dirname(fpath))
            os.write(h, fString)
            os.close(h)
            os.rename(fpath, fpath + '.tmp')
            os.rename(tmpFile, fpath)
            os.unlink(fpath + '.tmp')
