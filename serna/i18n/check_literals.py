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

import sys, os, re, datetime
    
src_re = re.compile(r"^.*\.((cxx)|(h))$", re.I)
skip_re = re.compile(r"^\s*((#\s*include\s+)|(//)).*$")
start_skip = re.compile(r"^\s*//\s*START_IGNORE_LITERALS.*$")
stop_skip = re.compile(r"^\s*//\s*STOP_IGNORE_LITERALS.*$")
complete_line_re = re.compile(r'^.*([\;\}\{])\s*$')
literal_re = re.compile(r'(?P<literal>(?<!\\)".*?(?<!\\)")+')

allowed = [
    r"tr",
    r"QT_TR(ANSLATE)?_NOOP",
    r"NOTR",
    r"get(Safe)?Property",
    r"makeDescendant",
    r"RT_MSG_ASSERT",
    r"REGISTER_COMMAND_EVENT_MAKER",
    r"GD",
    r"DYNCALL\d*",
    r"(Q|get)Color",
    r"QRegExp",
    r"translate",
    r"get_translated",
    r"inherits"
]

allowed_re = re.compile(r"^(" + '|'.join(allowed) + ")$")
macro_begin = re.compile(r"^(.*\W)?(?P<name>\w+)\s*\($")

def dump(lines, line):
    if 0 < len(lines):
        return ''.join(lines)
    else:
        return line
        
def linestrip(line):
#    line = re.sub(r'"\s*"', '', line)
    line = re.sub(r'\\.', '', line)
    line = re.sub(r"'\"'", '', line)
    return line
    
def eligible(line):
    line = line.strip('" \t\n')
    if re.match(r"^[\.\s\d]*$", line): return False
    if re.match(r"^\W*$", line): return False
    if re.match(r"^.$", line): return False
    return True
    
ddbg_re = re.compile(r"^.*((IMDBG)|(DDBG)|((std::)?c(err|out)))\s*<<\s*")
dbg_re = re.compile(r"^.*DBG\s*\(.+?\)\s*<<\s*")
dbgexec_re = re.compile(r"^.*DBG_EXEC.*$")
dbgif_re = re.compile(r"^.*DBG_IF.*(\)\s*{)?.*$")

def is_debug(line):
    d_obj = ddbg_re.match(line)
    if d_obj:
        return True
    d_obj = dbg_re.match(line)
    if d_obj:
        return True
    d_obj = dbgif_re.match(line)
    if d_obj:
        return True
    d_obj = dbgexec_re.match(line)
    if d_obj:
        return True
    return False

def find_allowed_wrapper(line, start, end):
    if is_debug(line[:start]):
        return "DBG"
        
    lhp = line.rfind('(', 0, start)
    rhp = line.find(')', end) + 1
    if 0 < lhp and 0 < rhp:
        m_obj = macro_begin.match(line, 0, lhp + 1)
        if m_obj:
            wrapper = m_obj.group("name")
            if allowed_re.match(wrapper):
                return wrapper
        return find_allowed_wrapper(line, lhp, rhp)
    return ""

def do_check(source):
    lines = []
    lineno = 0
    dbg_flag = ''
    skip_flag = 0
    comment_flag = 0
    mismatches = []
    for line in iter(open(source)):
        lineno += 1

        line = line.strip(" \t")
        
        start_comment = line.find("/*")
        end_comment = line.rfind("*/")
        if -1 != start_comment:
            if end_comment < start_comment:
                comment_flag = lineno
            else:
                line = line[:start_comment] + ' '*(end_comment-start_comment) + line[end_comment:]
        
        if -1 != end_comment:
            comment_flag = 0
            
        if comment_flag: continue
                
        if stop_skip.match(line):
            if not skip_flag:
                print >> sys.stderr, "STOP_IGNORE_LITERALS encountered at "\
                      "line %d when not skipping" % lineno
            skip_flag = 0
            continue
            
        if start_skip.match(line):
            if skip_flag:
                print >> sys.stderr, "START_IGNORE_LITERALS encountered "\
                      "twice at lines %d and %d", skip_flag, lineno
            skip_flag = lineno
            continue
            
        if skip_flag or skip_re.match(line): continue

        line = re.sub(r"\s*//+.*$", "", line)
        
        complete_line_obj = complete_line_re.match(line)
        if not complete_line_obj:
            lines.append(line)
            continue
        elif 0 < len(lines):
            lines.append(line)
            line = ""
            for l in lines:
                line += l.rstrip('\\\n').strip()
            
        line = linestrip(line)
        start = 0
        columns = []
        for m_obj in literal_re.finditer(line):
            if not eligible(m_obj.group("literal")): continue
            start = m_obj.start("literal")
            wrapper = find_allowed_wrapper(line, start, m_obj.end("literal"))
            if 0 < len(wrapper):
                continue
            
            columns.append(start)
            
        if len(columns):
            mismatches.append((lineno, columns, dump(lines, line), line))
        
        lines = []
        
    cnt = len(mismatches)
    if cnt:
        for linenum, cols, linedump, complete_line in mismatches:
            columns = ','.join([ str(c) for c in cols ])
            print "ERROR: Untranslated literal in %s:%d" % (source, linenum)
            print "      ", linedump
        
    if 0 < skip_flag:
        print >> sys.stderr, "Unmatched START_IGNORE_LITERALS in %s:%d" \
                 % (source, skip_flag)
        
    return cnt

def pathgen(path):
    for dp, dlist, flist in os.walk(path):
        for src in [ f for f in flist if src_re.match(f) ]:
            yield os.path.join(dp, src)
    raise StopIteration

if len(sys.argv) < 2:
    print "\nUsage: %s <src1> [<src2>..<srcN>]" % sys.argv[0]
    print """\
The script checks source files for bare literals which do not appear
as arguments for a set of special macros and functions

To exclude parts of source file from checking, enclose it between 
// START_IGNORE_LITERALS 
line and
// STOP_IGNORE_LITERALS
line. 

Currently all macro and function names that match the regular expression 
%s
exclude its bare literal arguments from being reported by this script
""" % allowed_re.pattern
    
def check(src):
    if not src_re.match(src):
        return
        
    if not os.path.exists(src):
        print >> sys.stderr, "Source file '%s' does not exist" % src
        sys.exit(-1)
        
    start = datetime.datetime.now()
    cnt = do_check(src)

print "\nStart checking translation of literals ..."
file_count = 0

for src in sys.argv[1:]:

    if '@' == src[0]:
        for srcfile in file(src[1:], "r").read().strip().split():
            check(srcfile)
            file_count += 1
    elif os.path.isdir(src):
        for path in pathgen(src):
            check(path)
            file_count += 1
    else:
        check(src)
        file_count += 1

print "End checking translation of literals. ", file_count, "file(s) checked\n"
