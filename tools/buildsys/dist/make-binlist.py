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

import os, re, sys
from subprocess import *

def usage():
    print >> sys.stderr, "Usage: %s <source_listfile> <binlist_file>"
    sys.exit(-1)
    
if len(sys.argv) < 3:
    usage()

exe_re = re.compile(r"(.*\.so$)|(.*\.so\..*)|(lib.*)")
py_re = re.compile(r"^.*\.py[co]$")
binlist = []

for line in file(sys.argv[1], "r"):
    fullpath = line.strip()
    while os.path.islink(fullpath):
        fullpath = os.path.join(os.path.dirname(fullpath), os.readlink(fullpath))
    fullpath = os.path.normpath(fullpath)
    if not os.path.exists(fullpath) and not py_re.match(fullpath.lower()):
        print >> sys.stderr, "File '%s' does not exist" % fullpath
        sys.exit(-1)
    path, name = os.path.split(fullpath)
    if exe_re.match(name) or os.access(fullpath, os.X_OK):
        cmd = "file " + fullpath
        filecmd = Popen(cmd, shell=True, stdout=PIPE, stderr=STDOUT, close_fds=True)
        res = filecmd.stdout.read()
        if -1 != res.find("ELF") or -1 != res.find("/bin/sh"):
            binlist.append(fullpath)

file(sys.argv[2], "w+").write('\n'.join(binlist))
