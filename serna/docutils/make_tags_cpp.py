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
    print >> sys.stderr, "<tag_list_file> <cpp_file>"
    sys.exit(1)

if len(sys.argv) < 2:
    usage()

def read_tags(tags_file):
    tags_dict = {}
    for line in open(tags_file, "r"):
        line = line.strip()
        tag, url = [ x.strip() for x in line.split() ]
        tags_dict.setdefault(tag, []).append(url)
    return tags_dict

tags_dict = read_tags(sys.argv[1])

tags = tags_dict.keys()
tags.sort()
result = True
pairs = []

for tag in tags:
    urls = tags_dict[tag]
    if len(urls) > 1:
        print >> sys.stderr, "Duplicate tag found: %s maps to '%s" % (tag,
                             "' and '".join(urls) + "'")
#        result = False
    pairs.append('\t{ "%s", "%s" },' % (tag, urls[0]))

if result:
    outfile = open(sys.argv[2], "w+")
    print >> outfile, """
#include "docutils/impl/DocTag.h"

namespace doctags {

const DocTag DOCTAGS[] = {"""
    print >> outfile, "\n".join(pairs)
    print >> outfile, '\t{ 0, 0 }\n};\n\n// %d' % len(tags)
    print >> outfile, "const int DOCTAGS_SIZE = sizeof(DOCTAGS)/sizeof(DOCTAGS[0]) - 1;"
    print >> outfile, "\n}"
else:
    sys.exit(-1)
