## 
## Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
## Contact: info@syntext.com, http://www.syntext.com
## 
## This file is part of Syntext Serna XML Editor.
## 
## COMMERCIAL USAGE
## Licensees holding valid Syntext Serna commercial licenses may use this file
## in accordance with the Syntext Serna Commercial License Agreement provided
## with the software, or, alternatively, in accorance with the terms contained
## in a written agreement between you and Syntext, Inc.
## 
## GNU GENERAL PUBLIC LICENSE USAGE
## Alternatively, this file may be used under the terms of the GNU General 
## Public License versions 2.0 or 3.0 as published by the Free Software 
## Foundation and appearing in the file LICENSE.GPL included in the packaging 
## of this file. In addition, as a special exception, Syntext, Inc. gives you
## certain additional rights, which are described in the Syntext, Inc. GPL 
## Exception for Syntext Serna Free Edition, included in the file 
## GPL_EXCEPTION.txt in this package.
## 
## You should have received a copy of appropriate licenses along with this 
## package. If not, see <http://www.syntext.com/legal/>. If you are unsure
## which license is appropriate for your use, please contact the sales 
## department at sales@syntext.com.
## 
## This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
## WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
## 
#!/usr/bin/python

import os, sys, shutil

if len(sys.argv) < 3:
    print >> sys.stderr, "Usage: %s <package_dir> <manifest_file>" % sys.argv[0]
    print >> sys.stderr, "checks that <package_dir> does not contain files not specified in <manifest>"
    print >> sys.stderr, "<manifest> contains a list of absolute file paths separated by line-breaks"
    sys.exit(-1)

pkgdir, manifest = sys.argv[1:3]
pkgdir = os.path.normpath(os.path.abspath(pkgdir)).strip()
error = False
dstfiles, dstdirs = {}, {}

for x in open(manifest, "rb").readlines():
    normpath = os.path.normpath(x).strip()
    if not os.path.exists(normpath):
        print >> sys.stderr, "'%s' is in '%s' but doesn't exist" % (normpath, manifest)
        error = True
        continue
    if os.path.isfile(normpath):
        dstfiles[normpath] = 1
        normpath = os.path.dirname(normpath).strip()
    dstdirs[normpath] = 1

dstdirs[pkgdir] = 1

for dd in dstdirs.keys():
    parent = dd
    while True:
        parent = os.path.dirname(parent)
        if len(parent) > len(pkgdir):
            dstdirs[parent] = 1
        else:
            break

for top, dirs, names in os.walk(pkgdir):
    for name in names:
        dstfile = os.path.join(top, name)
        if dstfile in dstfiles:
            continue
        print "%s is not in %s, removing" % (dstfile, manifest)
        os.unlink(dstfile)
    for d in [ x for x in dirs if '.' != x and '..' != x ]:
        dstdir = os.path.join(top, d)
        if dstdir in dstdirs:
            continue
        print "%s is not in %s, removing" % (dstdir, manifest)
        shutil.rmtree(dstdir, True)

if error:
    sys.exit(-1)
