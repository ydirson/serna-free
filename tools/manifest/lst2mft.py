#!/usr/bin/python
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

import os, sys, getopt, re

def usage():
    print >> sys.stderr, "usage: " + sys.argv[0],
    print >> sys.stderr, "-p <number of path components to strip>"
    print >> sys.stderr, "-s <srcdir> -d <dstdir> -o <outfile> <inputfile>"
    sys.exit(1)

srcdir = dstdir = ''
outfile = '-'
dst_strip = basestrip = pathstrip = 0
excludeListFiles = []
excludePatterns = []

try:
    opts, args = getopt.gnu_getopt(sys.argv[1:], "p:s:d:o:X:b:D:x:", \
                                   ['pathstrip=', 'srcdir=', 'dstdir=', \
                                    'output=', 'exclude=', 'exclude-from-file='])
except getopt.GetoptError:
    usage()

for o, a in opts:
    if o in ('-o', '--output'):
        outfile = a
    elif o in ('-p', '--pathstrip'):
        pathstrip = int(a)
    elif o in ('-s', '--srcdir'):
        srcdir = a.strip("'\"")
    elif o in ('-d', '--dstdir'):
        dstdir = a.strip("'\"")
    elif o in ('-X', '--exclude-from-file'):
        excludeListFiles.append(a.strip("'\""))
    elif o in ('-x', '--exclude'):
        excludePatterns.append(re.compile(a.strip("'\"")))
    elif o in ('-b', '--basestrip'):
        basestrip = int(a.strip("'\""))
    elif o in ('-D', '--dststrip'):
        dst_strip = int(a.strip("'\""))
    else:
        usage()

for listFile in excludeListFiles:
    for line in open(listFile, 'r'):
        line = line.strip()
        if line:
            excludePatterns.append(re.compile(line.strip()))
        
def exclude(line):
    if line.endswith('/'):
        return True
    for pattern in excludePatterns:
        if pattern.search(line):
            return True
    return False

def dump_mft(outfile, list):
    mft = {}

    is_win32 = sys.platform.startswith('win32')

    for line in list:
        if exclude(line):
            continue
        if pathstrip:
            paths = line.split('/')
            del paths[:pathstrip]
            line = '/'.join(paths)
        
        mode = 0644
        prefix, basename = os.path.split(line)
        path_dict = mft.setdefault(prefix.strip('/'), {})
        key = 'data'
        if not is_win32:
            if os.path.exists(line) and os.access(line, os.X_OK):
                key = 'programs'
        path_dict.setdefault(key, []).append(basename)
    
    paths = mft.keys()
    paths.sort()

    mft_entries = []
    for path in paths:
        path_dict = mft[path]
        src_prefix = os.path.join(srcdir, path).replace('\\', '/').strip('/')
        if dst_strip:
            dirs = re.split(r'[/\\]', path)
            dst_path = '/'.join(dirs[dst_strip:])
        else:
            dst_path = path
        dst_prefix = os.path.join(dstdir, dst_path).replace('\\', '/').strip('/')
        for ptype in ('data', 'programs'):
            names = path_dict.get(ptype, [])
            if not names:
                continue
            names.sort()
            mft_entries.append("%s:%s:%s" % (ptype, dst_prefix, src_prefix))
            for name in names:
                mft_entries.append(' '* 4 + name)
            mft_entries.append('')

    if mft_entries:
        if '-' == outfile:
            outfp = sys.stdout
        else:
            outfp = open(outfile, "w")
        outfp.write('\n'.join(mft_entries))
        
def make_list_from_args(args):
    list = []
    for infile in args:
        if '-' == infile:
            infp = sys.stdin
        else:
            infp = open(infile, "r")
        for line in infp:
            line = line.strip().replace('\\', '/').lstrip('/')
            list.append(line)
    return list
            
def make_list_from_path(path, pstrip=0):
    list = []
    num_paths = len(re.split(r'[\\/]', path)) - pstrip
    if 0 > num_paths:
        num_paths = 0
    for top, dirs, fnames in os.walk(path):
        tops = re.split(r'[\\/]', top)
        newtop = os.sep.join(tops[num_paths:])
        for fname in fnames:
            entry = os.path.join(newtop, fname)
            list.append(entry)
    return list

if not args:
    args = ['-']

list = []

for arg in args:
    if '-' == arg or os.path.isfile(arg):
        list.extend(make_list_from_args([arg]))
    elif os.path.isdir(arg):
        list.extend(make_list_from_path(arg, basestrip))
    
dump_mft(outfile, list)
