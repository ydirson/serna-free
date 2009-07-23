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
#
# Compile the binary resource(s) into the C++ representation
#
import os, sys, codecs, struct
import codecs, getopt

listfile  = ""
basepath  = ""
outfile   = ""
utf8coder = codecs.getencoder("utf-8")
ostream   = sys.stdout
verbose   = 0

def pack_resource(sourcefile, resname):
    if verbose:
        print >> sys.stderr, "Packing resource: ", sourcefile, " as ", resname
    sf = file(sourcefile, "rb")
    data = sf.read()
    encoded_resname = utf8coder(resname)
    out_number(len(data))
    out_number(encoded_resname[1] + 1)
    out_bytestring(encoded_resname[0])
    print >> ostream, "0, "
    out_bytestring(data)

def out_number(n):
    for i in range(0, 4):
        print >> ostream, "0x" + "%02x" % ((n >> (i*8)) & 0377) + ",",
    print >> ostream

def out_bytestring(s):
    for i in range(len(s)): 
        print >> ostream, "0x" + "%02x" % struct.unpack('B', s[i]) + ",",
        if ((i + 1) % 8) == 0:
            print >> ostream

opts, args = getopt.getopt(sys.argv[1:], "l:b:o:v")
for opt, arg in opts:
    if opt == "-l":
        listfile = arg
    if opt == "-b":
        basepath = arg
    if opt == "-o":
        outfile = arg
    if opt == "-v":
        verbose = 1

if not listfile:
    print >> sys.stderr, "Resource list file must be specified\n";
    sys.exit(1)

if outfile:
    ostream = file(outfile, "w")

for f in file(listfile, "r").readlines():
    f = f.strip()
    if not f or f[0:1] == '#':
        continue
    bp = ""
    if len(basepath):
        bp = basepath + '/'
    else:
        bp = os.path.dirname(listfile) + '/'
    filename = ""
    if len(bp):
        filename = bp + '/' + f
    else:
        filename = f
    pack_resource(filename.split()[0], f)

out_number(0)
print >> ostream
