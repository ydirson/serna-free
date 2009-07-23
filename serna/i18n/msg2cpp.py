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
#
# 
#

import sys, string, os

def usage():
    print >> sys.stderr, "Usage: " + sys.argv[0] + " source.msg [output_file]"
    sys.exit(-1)

if len(sys.argv) < 2:
	usage()
if not os.access(sys.argv[1], os.F_OK | os.R_OK):
	print >> sys.stderr, sys.argv[1], "is not readable or doesn't exist"
	usage()

try:
    module_name = os.path.splitext(os.path.split(sys.argv[1])[1])[0]
    if len(sys.argv) == 3:
        outfile_name = sys.argv[2]
    else:
        outfile_name = module_name + ".cpp"
    sys.stdout = open(outfile_name, "w+")

    print "/// THIS FILE IS GENERATED AUTOMATICALLY. DO NOT EDIT."
    #print "void " + module_name + "::translate() {" 
    print "void msg::translate() {" 

    for line in open( sys.argv[1], "r").readlines():
        line = line.rstrip()

        if len(line) ==0 or line[0] == '#':
            continue            
        if line[0] in string.digits:
            continue

        msg_name, msg_text = line.split(None, 1)
        print ("tr(\"" + msg_text.strip() + "\", \"" +
               module_name + " | " + msg_name.strip() + "\");")

    print "};"
    print "/// END OF GENERATED FILE"

except:
	raise
