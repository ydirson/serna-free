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
    print >> sys.stderr, "Usage: %s <check XSL script> <path to 3rd party directory> <ts_file> [ts_file...]" % sys.argv[0]
    print >> sys.stderr, "The script checks every <ts_file> for unfinished and obsolete entries"
    sys.exit(-1)
    
if len(sys.argv) < 4:
    usage()

unf_re = re.compile(r'<translation type="unfinished">')
obs_re = re.compile(r'<translation type="obsolete">')

print "\nStart checking TS files ..."
file_count = 0
xsltproc = ""
script = ""
file_list = []

if '-' == sys.argv[3]:
    file_list = sys.stdin.read().strip().split(' ')
else:
    file_list = sys.argv[3:]

for f in file_list:
    if f.endswith("_untranslated.ts"):
        continue
    file_count += 1
    contents = file(f, "r").read()
    err = ''
    if unf_re.search(contents, re.M):
        err += 'unfinished'
    if 0 != len(err):
        print "ERROR: unfinished translations in file", f
        if not xsltproc:
            script, third_dir = [ re.sub(r"[/\\]", os.path.sep*2, x) for x in sys.argv[1:3] ]
            platform = sys.platform
            if platform.startswith("linux"): platform = "linux"
            xsltproc = os.path.sep.join((third_dir, "bin", "xsltproc"))
        cmd = ' '.join((xsltproc, script, f))
        cin, coe = os.popen4(cmd)
        cin.close()
        xslt_output = coe.read()
        print xslt_output
        print

print "End checking TS files. ", file_count, "file(s) checked\n"
