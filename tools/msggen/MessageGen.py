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

import sys, os, string

copyright = """// Copyright (c) Syntext Inc.
// This is a copyrighted commercial software. See COPYRIGHT file for details.
//
// THIS FILE WAS GENERATED AUTOMAgICALLY BY MSGGEN.
// ANY MANUAL CHANGES TO THIS FILE WILL BE LOST.
"""

#SWAPBYTES = sys.platform != "win32"
SWAPBYTES=0
WCHAR_T = "unsigned short"

def unicode_bytes(char):
    uint = ord(char)
    high = (uint & 0xffff0000) >> 8
    low = uint & 0xffff
    if SWAPBYTES:
        return (low, high)
    return (high, low)

def dump_message( module_id, msg_id, message):
    mod_msg = str(module_id) + "_" + str(msg_id)
    array = "sv" + mod_msg
    print "\t" + WCHAR_T, array, "[] = {",
    for char in message:
        print "0x%02x%02x," % unicode_bytes( char),
    print "0 };"
    print "\tCoreMessageFetcher::_msg msg%s = { %d, %s};" % (mod_msg, len( message), array)
    return "msg" + mod_msg

def dump_message2( module_id, msg_id, message):
    mod_msg = str(module_id) + "_" + str(msg_id)
    array = "sv" + mod_msg
    print "static const " + "CompiledInStrings::UCharType", array, "[] = {",
    unicode_message = unicode(message, 'utf-8')
    for char in unicode_message:
        print "0x%02x%02x," % unicode_bytes(char),
    print "0 };"
    print "static const CompiledInStrings::_msg msg%s = { %d, %s};" % (mod_msg, len( message), array)
    return "msg" + mod_msg

def printStartNamespace( name = ""):
    print "namespace %s {" % (name.strip())
#    print "USING_COMMON_NS"

def printEndNamespace( name = ""):
    print "} // namespace %s\n" % (name)

class DuplicateIdError( Exception):
    def __init__( self, msg_id, msg_name):
        Exception.__init__( self, msg_name)
        self.msg_id_ = msg_id

class GenBase:
    def __init__( self, lang = "EN", cxx_mod = None):
        self.lang_ = lang
        self.cxx_module_ = cxx_mod
        self.module_name_ = None
        self.module_id_ = -1
        self.msg_id_ = -1
        self.msg_map_ = {}

    def startModule( self, mod_id, mod_name):
        self.module_id_ = int( mod_id)
        self.module_name_ = str( mod_name)

    def endModule( self):
        self.msg_map_ = {}
        return

    def startMessage( self, msg_id, msg_name):
        self.msg_id_ = msg_id
        if self.msg_map_.has_key( msg_id):
            raise DuplicateIdError( msg_id, msg_name)
        else:
            self.msg_map_[msg_id] = 1
        return

    def endMessage( self, name):
        return

    def messageText( self, text, lang):
        return

class CppGen( GenBase):
    def __init__( self, lang = "EN"):
        self.text_ = {}
        self.msg_pointers_ = {}

        GenBase.__init__( self, lang)

    def startModule( self, mod_id, mod_name):
        print copyright
        print "#include <common/MessageUtils.h>\n"
        printStartNamespace()
        GenBase.startModule( self, mod_id, mod_name)

    def endModule( self):
        msg_ids = self.msg_pointers_.keys()
        highest_id = max( msg_ids)
        if highest_id != len(msg_ids):
            print >> sys.stderr, "Warning: non-contiguous message identifiers"

        msgv, seg, cmf_reg = map( lambda x, y=str(self.module_id_): x + y, ("msgv", "seg", "cmf_reg"))
        print "\n\tCoreMessageFetcher::_msg* %s[] = {" % (msgv),
        for msg_id in xrange(1, highest_id+1):
            if self.msg_pointers_.has_key( msg_id):
                print "&%s," % ( self.msg_pointers_[msg_id]),
            else:
                print "0,",

        print "0 };"
        print "\tCoreMessageFetcher::_seg",
        print "%s = { %d, %s};\n" % (seg, highest_id, msgv)
        print "\tconst CoreMessageFetcher::Registrar",
        print "%s( %d, &%s);\n" % ( cmf_reg, self.module_id_, seg)
        printEndNamespace()
        printStartNamespace( self.module_name_)
        dllexport = ""
        if sys.platform == "win32": dllexport = "__declspec(dllexport) "
        print "\t", dllexport + "void cmf_handle" + str(self.module_id_) + "() {}"
        printEndNamespace( self.module_name_)
        print "// END OF AUTOMAgICALLY GENERATED FILE"

    def endMessage( self, name):
        self.msg_pointers_[self.msg_id_] = dump_message( self.module_id_, self.msg_id_, self.text_[self.lang_].strip())
        self.text_ = {}

    def messageText( self, text, lang = "EN"):
        if self.text_.has_key( lang):
            self.text_[lang] += text
        else:
            self.text_[lang] = text

class HeaderGen( GenBase):
    def __init__( self, lang = "EN"):
        GenBase.__init__( self, lang)

    def startModule( self, mod_id, mod_name):
        GenBase.startModule( self, mod_id, mod_name)

        print r"#ifndef " + self.module_name_ + "_MSGGEN"
        print r"#define " + self.module_name_ + "_MSGGEN\n"
        print copyright
        printStartNamespace( self.module_name_)
        print "\tenum MessageId {"
        print "\t\tRESERVED_ELEMENT = 0, // Used in algorithm implementation and must be 0"

    def endModule( self):
        print "\t\tnMessages\n\t};"
	cmf_handle_func = "cmf_handle" + str( self.module_id_)
	print "\tvoid", cmf_handle_func + "();"
        printEndNamespace( self.module_name_)
        printStartNamespace()
	print "\ttypedef void (*pf_cmf) (void);";
	print "\tconst pf_cmf", cmf_handle_func, "= " + self.module_name_ + "::" + cmf_handle_func + ";"
        printEndNamespace()
        print "// END OF AUTOMAgICALLY GENERATED FILE"
        print r"#endif // " + self.module_name_ + "_MSGGEN\n"

    def startMessage( self, msg_id, msg_name):
        print "\t\t%s = 0x%02x%04x," % ( msg_name, self.module_id_, msg_id)

class NextIdGen( GenBase):
    def __init__( self, lang = "EN"):
        GenBase.__init__( self, lang)
        self.max_msg_id_ = 0

    def startMessage( self, msg_id, msg_name):
        self.max_msg_id_ = max( self.max_msg_id_, msg_id)

    def endModule( self):
        print >> sys.stderr, self.max_msg_id_ + 1

try:
    import xml.sax.handler

    class MsgHandler( xml.sax.ContentHandler):
        def __init__( self, handler, lang = "EN"):
            self.handler_ = handler
            xml.sax.ContentHandler.__init__( self)
            self.lang_ = lang

        def startElement( self, name, attrs):
            if name == "messages":
                self.handler_.startModule( int( attrs["moduleId"]), attrs["module"])
            elif name == "message":
                self.handler_.startMessage( int( attrs["id"]), attrs["textid"])
            elif name == "text":
                self.lang_ = attrs["lang"]

        def endElement( self, name):
            if name == "messages":
                self.handler_.endModule()
            elif name == "message":
                self.handler_.endMessage( name)

        def characters( self, text):
            self.handler_.messageText( text, self.lang_)
except:
    pass

class MSGParser:
    def __init__( self, handler, lang = "EN"):
        self.handler_ = handler
        self.lang_ = lang


    def parse( self, name):
        module_name = os.path.split( os.path.splitext( name)[0])[1]
        msg_id = 0
        module_started = 0
        for line in open( name, "r").readlines():
            line = line.rstrip()
	    if 0 == len( line): continue
            if line[0] == '#':
                continue
            elif line[0] in string.digits:
                if module_started:
                    self.handler_.endModule()
                self.handler_.startModule( line, module_name)
                module_started = 1
            else:
                msg_name, msg_text = line.split(None, 1)
                msg_id += 1
                self.handler_.startMessage( msg_id, msg_name)
                self.handler_.messageText( msg_text, self.lang_)
                self.handler_.endMessage( msg_id)

        self.handler_.endModule()
