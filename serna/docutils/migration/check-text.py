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

class textfile:
    wordre = re.compile(r"\w+", re.M)
    def __init__(self, name):
        self.name_ = name
        self.stat_ = os.stat(self.name_)

    def __getattr__(self, name):
        if name == 'st_name':
            return self.name_.replace('.text', '')
        elif name == 'st_words':
            if not hasattr(self, "wordcount_"):
                self.wordcount_ = self.count_words()
            return self.wordcount_
        elif hasattr(self.stat_, name):
            return getattr(self.stat_, name)
        raise AttributeError

    def unlink(self):
        os.unlink(self.name_)

    def count_words(self):
        return len(self.wordre.findall(open(self.name_, "r").read()))

mark = "  Wrote to "

def pct(x,y): return abs(100*(x-y)/x)

for line in sys.stdin:
    if line.startswith(mark):
        lhs, rhs = [ textfile(x.strip())
                     for x in line[len(mark):].split(',') ]
        if lhs.st_size and rhs.st_size:
            diff = pct(lhs.st_size, rhs.st_size)
            print '\n'.join(["%s%s%s%s"]*3) % tuple([str(x).ljust(16) for x in \
                (' ', lhs.st_name, rhs.st_name, 'diff(%)',
                 'bytes', lhs.st_size, rhs.st_size, diff,
                 'words', lhs.st_words, rhs.st_words,
                    pct(lhs.st_words, rhs.st_words))])
            print
            lhs.unlink()
            rhs.unlink()
            if diff > 1:
                print >> sys.stderr, "Difference is %d%%" % diff
                sys.exit(-1)


