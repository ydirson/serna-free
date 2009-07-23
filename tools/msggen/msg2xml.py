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
    print >> sys.stderr, "Usage: " + sys.argv[0] + " source.msg"
    sys.exit(-1)

subst = [('&', "&amp;"),('"', "&quot;"),("'", "&apos;"),('<', "&lt;"),('>', "&gt;")]
msg_id = 0
comment = None

if len( sys.argv) < 2:
	usage()
if not os.access( sys.argv[1], os.F_OK | os.R_OK):
	print >> sys.stderr, sys.argv[1], "is not readable or doesn't exist"
	usage()

try:
    module_name = os.path.splitext( os.path.split( sys.argv[1])[1])[0]
    sys.stdout = open( module_name + ".xml", "w+")

    print '<?xml version="1.0"?>'

    for line in open( sys.argv[1], "r").readlines():
        line = line.rstrip()
        if line[0] == '#':
            if comment is None:
                print "<!--"
                comment = 1
            print line[1:]
            continue
        elif not comment is None:
            comment = None
            print "-->"
            
        if line[0] in string.digits:
            print '<messages moduleId="' + line + '" module="' + module_name + '">'
            continue

        msg_name, msg_text = line.split(None, 1)
        for rep, sub in subst:
            msg_text = msg_text.replace( rep, sub);

        msg_id += 1
        print "\t",     '<message id="' + str(msg_id) + '" textid="' + msg_name + '">'
        print "\t\t",   '<text lang="EN">'
        print "\t\t\t", msg_text
        print "\t\t",   '</text>'
        print "\t",     '</message>'

    print '</messages>'
except:
	raise
