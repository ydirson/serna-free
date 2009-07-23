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
#!/usr/local/bin/python
#
# 
#
import sys, string
import os, os.path
import getopt
import xml.sax, xml.sax.handler

import MessageGen, MessageGen2

class ImproperUsage:
    pass

def usage():
    print >> sys.stderr, "Usage: " + sys.argv[0] + " [-c|-h|-n|-C|-H] -M cxx_lib_name destination.{cpp|h} source.{xml|msg}"
    sys.exit(-1)
    
def cleanup(status, msg, fobj = sys.stdout):
    print >> sys.stderr, msg
    name = fobj.name
    fobj.close()
    if os.access(name, os.W_OK):
	os.unlink(name)
    sys.exit(exitstat)

try:
    CodeGen = None
    module = None
    opts, args = getopt.getopt(sys.argv[1:], "hcnCHM:")
    for opt, arg in opts:
        if opt == "-h" and CodeGen is None:
            CodeGen = MessageGen.HeaderGen()
        elif opt == "-c" and CodeGen is None:
            CodeGen = MessageGen.CppGen()
        elif opt == "-H" and CodeGen is None:
            CodeGen = MessageGen2.HeaderGen2()
        elif opt == "-C" and CodeGen is None:
            CodeGen = MessageGen2.CppGen2()
        elif opt == "-n" and CodeGen is None:
            CodeGen = MessageGen.NextIdGen()
        elif opt == "-M":
            module = arg
        else:
            raise ImproperUsage

    if module is None:
        raise ImproperUsage
    CodeGen.cxx_module_ = module
    if len(args) < 2:
        raise ImproperUsage

    if not os.access(args[1], os.F_OK | os.R_OK):
        print >> sys.stderr, args[1], "is not readable or doesn't exist"
        raise ImproperUsage

    ext = os.path.splitext(args[1])[1].lower()
    if ext == ".xml":
        try:
            parser = xml.sax.make_parser()
        except:
            import SSXMLParser
            parser = SSXMLParser.SSXMLParser()

        parser.setContentHandler(MessageGen.MsgHandler(CodeGen))
    elif ext == ".msg":
        parser = MessageGen.MSGParser(CodeGen)
    else:
        print >> sys.stderr, args[1], "has neither XML nor MSG extension"
        raise ImproperUsage

    sys.stdout = open(args[0], "w+");
    parser.parse(args[1])

except ImproperUsage:
    usage()
except MessageGen.DuplicateIdError, dup_exception:
    cleanup(2, 'Error: duplicate id: "' + str(dup_exception.msg_id_) + '", message text id: "' + dup_exception.__str__() + '"')
except xml.sax.SAXException, saxException:
    cleanup(1, "SAX error occurred: '" + saxException.getMessage() + "'")
except getopt.GetoptError, getopterr:
    print >> sys.stderr, getopterr
    usage()
