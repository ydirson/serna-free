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

import sys, os, re

def usage():
    print >> sys.stderr, "\nUsage:", sys.argv[0],
    print >> sys.stderr, "<tag_list_file> <src_path> [<src_path1> .. <src_pathN>]"
    sys.exit(1)

if len(sys.argv) < 3:
    usage()

def read_tags(tags_file):
    tags_dict = {}
    for line in open(tags_file, "r"):
        line = line.strip()
        tag, url = [ x.strip() for x in line.split() ]
        tags_dict.setdefault(tag, {}).setdefault(url, 1)
    return tags_dict

doctag_re = re.compile("^.*DOCTAG\(([^\)]*)\).*$")
comment_re = re.compile("^\s*//.*$")

reftags_dict = {}

def check_file(cxxfile, tags_dict):
    rv = True
    lineno = 0
    global reftags_dict
    for line in open(cxxfile, "r"):
        lineno = lineno + 1
        m_obj = doctag_re.match(line)
        if not m_obj is None:
            tag = m_obj.group(1)
            pos = "%s:%d" % (cxxfile, lineno)
            reftags_dict.setdefault(tag, []).append(pos)
            if not tag in tags_dict and comment_re.match(line) is None:
                print >> sys.stderr, "Unknown doctag '%s' referenced at '%s'" \
                    % (tag, pos)
                rv = False
    return rv

def check_files(path, files, tags_dict):
    rv = True
    for srcname in [ cxxfile for cxxfile in files if cxxfile.endswith('.cxx') ]:
        srcfullpath = os.path.join(path, srcname)
        rv = rv and check_file(srcfullpath, tags_dict)
    return rv
        
tags_dict = read_tags(sys.argv[1])

result = True

for srcpath in sys.argv[2:]:
    for root, dirs, files in os.walk(os.path.abspath(srcpath)):
        if 0 < len(files):
            result = result and check_files(root, files, tags_dict)
        if ".svn" in dirs:
            dirs.remove(".svn")

reftags_list = reftags_dict.keys()
reftags_list.sort()
reftags_file = open("reftags.lst", "w+")

for tag in reftags_list:
    print >> reftags_file, "%s:" % tag
    for src in reftags_dict[tag]:
        print >> reftags_file, "\t%s" % src

for tag, urls in tags_dict.iteritems():
    if len(urls) > 1:
        print >> sys.stderr, "Duplicate doctag '%s' maps to '%s" % (tag,
                             "' and '".join(urls.keys()) + "'")

if not result:
    sys.exit(1)

