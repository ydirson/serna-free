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

import sys, os, glob, fnmatch

def usage():
    print >> sys.stderr, "\nUsage:", sys.argv[0],
    print >> sys.stderr, "<manifest_file> <doc_path> [<manifest_file1> <doc_path1> .. <manifest_fileN> <doc_pathN>]"
    sys.exit(1)

def read_dir(path, pattern = '*'):
    files = [ os.path.basename(x.strip()) \
              for x in glob.glob(os.path.join(path, pattern)) ]
    return dict(zip(files, [1] * len(files)))

def read_mft(mft, pattern):
    rv = {}
    lineno = 0
    for line in open(mft, "r"):
        lineno = lineno + 1
        if ' ' != line[0] or not fnmatch.fnmatch(line.strip(), pattern):
            continue
        rv[line.strip()] = lineno
    return rv

def check_mft(mft, gendir):
    rv = True
    mft_dict = read_mft(mft, "*.html")
    gen_dict = read_dir(gendir, "*.html")
    for m in mft_dict.keys():
        if not m in gen_dict:
            print >> sys.stderr, "Manifest file '%s:%d' contains '%s' which is not present in '%s'" \
                                  % (mft, mft_dict[m], m, gendir)
            rv = False
    for m in gen_dict.keys():
        if not m in mft_dict:
            print >> sys.stderr, "File '%s' generated in '%s' is not listed in manifest file '%s'" \
                                 % (m, gendir, mft)
            rv = False
    return rv
        
if len(sys.argv) < 3 or 0 == len(sys.argv) % 2:
    usage()

result = True

for (mft, gendir) in [ (sys.argv[i], sys.argv[i+1]) \
                      for i in range(1, len(sys.argv) - 1, 2) ]:
    result = check_mft(mft, gendir) and result

if not result:
    sys.exit(1)
