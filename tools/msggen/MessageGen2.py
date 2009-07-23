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

from MessageGen import *

class CppGen2( GenBase):
    def __init__( self, lang = "EN", namespace = ""):
        self.text_ = {}
        self.msg_pointers_ = {}

        GenBase.__init__( self, lang)
        self.namespace_ = namespace

    def startModule( self, mod_id, mod_name):
        print copyright
        print r'#include "common/common_defs.h"'
        print r'#include "common/common_types.h"'
        print r'#include "common/CompiledInStrings.h"' + '\n'
        print r'#include "' + mod_name + '.h"' + '\n'
#        printStartNamespace()
        print "USING_COMMON_NS\n"
        GenBase.startModule(self, mod_id, mod_name)

    def endModule( self):
        msg_ids = self.msg_pointers_.keys()
        highest_id = max(msg_ids)
        if highest_id != len(msg_ids):
            print >> sys.stderr, "Warning: non-contiguous message identifiers"

        msgv, seg, cmf_reg = map(lambda x, y=str(self.module_id_): x + y, ("msgv_"+self.lang_+"_", "seg_"+self.lang_+"_", "registrar_"))
        print "\nstatic const CompiledInStrings::_msg* %s[] = {" % (msgv),
        for msg_id in xrange(1, highest_id+1):
            if self.msg_pointers_.has_key( msg_id):
                print "&%s," % (self.msg_pointers_[msg_id]),
            else:
                print "0,",

        print "0 };"
        print "static const CompiledInStrings::_seg",
        print "%s = { %d, %s };\n" % (seg, highest_id, msgv)
        print "static const CompiledInStrings::Registrar",
        print '%s("en", %d, &%s);\n' % ( cmf_reg, self.module_id_, seg)
#        printEndNamespace()
#        print r'#if defined(_MSC_VER) && defined(_DLL)'
#        print r'# define EXPORT __declspec(dllexport)'
#        print r'#else'
#        print r'# define EXPORT'
#        print r'#endif' + '\n'
        print 'int', self.module_name_ + '::getFacility() { return', self.module_id_, "; }\n"
#        print "int EXPORT", self.module_name_ + "::forceLink() { return", self.module_id_, "; }\n"
        print "// END OF AUTOMAgICALLY GENERATED FILE"

    def endMessage( self, name):
        self.msg_pointers_[self.msg_id_] = dump_message2(self.module_id_, self.msg_id_, self.text_[self.lang_].strip())
        self.text_ = {}

    def messageText( self, text, lang = "EN"):
        if self.text_.has_key( lang):
            self.text_[lang] += text
        else:
            self.text_[lang] = text

class HeaderGen2( GenBase):
    def __init__( self, lang = "EN", namespace = ""):
        GenBase.__init__( self, lang)
        self.namespace_ = namespace

    def startModule( self, mod_id, mod_name):
        GenBase.startModule(self, mod_id, mod_name)

        print r"#ifndef " + self.module_name_ + "_MSGGEN"
        print r"#define " + self.module_name_ + "_MSGGEN\n"
        print copyright
        print r'#include "common/common_defs.h"'
        print r'#include "common/common_types.h"'
        print r'#include "common/Message.h"'
        print r'#include "'+self.cxx_module_.lower()+'/'+ self.cxx_module_.lower() + '_defs.h"' + '\n'
#        print r'#if defined(_MSC_VER) && defined(_DLL)'
#        print r'# define IMPORT __declspec(dllimport)'
#        print r'#else'
#        print r'# define IMPORT'
#        print r'#endif' + '\n'
        if self.namespace_ != "": printStartNamespace( self.namespace_)

        print "class", self.cxx_module_.upper() + "_EXPIMP", self.module_name_, ":",
        print "public COMMON_NS::MessageStream::UintMessageIdBase {"
        print "public:"
        print "\tenum Messages {"
        print "\t\tRESERVED_ELEMENT = 0, // Used in algorithm implementation and must be 0"

    def endModule( self):
        print "\t\tnMessages\n\t};"
        print "\tstatic int getFacility();\n"
#        print "\tenum { FACILITY =", self.module_id_, "};\n"
        print "\t" + self.module_name_ + "(Messages m) : UintMessageIdBase(m) {}"
#        handle_func = self.module_name_ + "_msg_handle_" + str( self.module_id_)
        print "};\n"
        if self.namespace_ != "": printEndNamespace(self.namespace_)

#        print "void", handle_func + "();\n"
#        printStartNamespace()
#        print "\ttypedef void (*pf_dummy_handler) (void);";
#        print "\tstatic const pf_dummy_handler", handle_func + "_ =", handle_func + ";"
#        print "static const int", self.module_name_ + "_link_enforcer =", self.module_name_ + "::forceLink();\n"
#        printEndNamespace()
        print r"#endif // " + self.module_name_ + "_MSGGEN\n"
        print "// END OF AUTOMAgICALLY GENERATED FILE"

    def startMessage( self, msg_id, msg_name):
        print "\t\t%s = 0x%06x," % ( msg_name, msg_id)
