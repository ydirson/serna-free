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
import sys, time, os

if len(sys.argv) < 7:
    print >> sys.stderr,"\nUsage: buildver.py <gen_file> <ver_file>",
    print >> sys.stderr, "<module_name> <product_name> <major> <minor> <patch>\n"
    sys.exit(1)

buildver, ver_file, module_name, product_name, major, minor, patch = sys.argv[1:8]

class BuildVer:
    def __init__(self, buildver, ver_file, module_name, product_name, major, minor, patch):
        self.buildver_ = buildver
        self.module_name_ = module_name
        self.product_name_ = product_name
        self.major_, self.minor_, self.patch_ = major, minor, patch
        self.basename_, ext = os.path.splitext(self.buildver_)

        self.text_patch_ = '0'

        try:
            for line in open(ver_file):
                line = line.strip()
                if line[0] != '#':
                    self.major_, self.minor_, self.text_patch_ = line.split('.', 2)
                    break
        except: 
            pass


        if "darwin" == sys.platform:
            self.extern_ = '__attribute__((visibility("hidden"))) extern'
        else:
            self.extern_ = 'extern'
        self.builddate_ = time.strftime("%Y%m%d")
    
    def dump_hpp(self):
        if self.module_name_ and self.product_name_ and self.major_ and self.minor_ and self.patch_:
            fobj = open(self.basename_ + ".hpp", "w+")

            print >> fobj, """#ifndef _VERSION_H\n#define _VERSION_H\n
# define MODULE_NAME \t\t"%(module_name_)s"
# define MODULE_PRODUCT_NAME \t"%(product_name_)s"\n
# define VERSION_MAJOR\t\t%(major_)s
# define VERSION_MINOR\t\t%(minor_)s
# define VERSION_PATCH\t\t%(patch_)s
# define BUILD_DATE\t\t%(builddate_)s

#ifndef RC_INVOKED\n
%(extern_)s const char buildver[];
%(extern_)s const char builddate[];\n
%(extern_)s const char modulename[];
%(extern_)s const char productname[];\n
#endif // RC_INVOKED"
#endif // _VERSION_H""" % self.__dict__
        else:
            sys.exit(1)

    def dump_cpp(self):
        if self.module_name_ and self.product_name_ and self.major_ and self.minor_ and self.patch_:
            fobj = open(self.basename_ + ".cpp", "w+")
            print >> fobj, """#include "buildver.hpp"\n
const char buildver[]    = "%(major_)s.%(minor_)s.%(patch_)s";
const char builddate[]   = "%(builddate_)s";
const char modulename[]  = "%(module_name_)s";
const char productname[] = "%(product_name_)s";\n""" % self.__dict__
        else:
            sys.exit(1)

try:
    bv = BuildVer(buildver, ver_file, module_name, product_name, major, minor, patch)
    bv.dump_cpp()
    bv.dump_hpp()
except:
    sys.exit(2)
