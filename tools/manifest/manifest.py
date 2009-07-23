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
#!/usr/bin/python2

import sys, string, getopt, os, re, copy

var_re = re.compile(r"\${(?P<var>\w[_\w\d]*)}")

class MFTError(Exception):
    def __init__(self, err):
        self.error = err
    def __str__(self):
        return self.error

def varsub(mobj, vdict):
    k = mobj.group("var")
    return vdict.get(k, mobj.group(0))

def is_absolute(fname):
    sfname = fname.strip()
    if "win32" == sys.platform:
        drive, tail = os.path.splitdrive(sfname)
        if 0 < len(drive) or sfname[0] in "\\/":
            return 1
    elif '/' == sfname[0]:
        return 1
    return 0
    
class Items:
    def __init__(self, vardict = {}):
        self.dstdirs_, self.prog_, self.data_ = {}, {}, {}
        self.sections_ = []
        self.makefile_variables_ = []
        self.skip_ = 0
        self.vardict_ = vardict
        if ("platform" in self.vardict_):
            self.platform_ = self_.vardict_["platform"]
        else:
            self.platform_ = sys.platform

    def read(self, infile):
        lc = 0
        for line in open(infile, "r").readlines():
            lc += 1
            sl = line.strip()
            if (0 == len(sl) or '#' == sl[0]):
                continue
            if ('[' == sl[0]):
                sect = self.resolve_variable(sl.strip("[]"))
                self.skip_ = not (self.platform_ok(sect) or
                                  self.section_ok(sect))
                continue
            if (self.skip_):
                continue
            if "include:" == sl[:8]:
                include_file = self.resolve_variable(sl[8:])
                
                if not is_absolute(include_file):
                    basedir = os.path.dirname(infile)
                    include_file = os.path.join(basedir, include_file)
                    
                if not os.path.exists(include_file):
                    raise MFTError("File '%s' included from '%s:(%d)' doesn't exist!" \
                                   % (include_file, infile, lc))
                else:
                    # We create a new object for items from included file
                    # because the skip_ and other flags should
                    # be local to the file.
                    included_items = copy.deepcopy(self)
                    included_items.read(include_file)
                    self.prog_.update(included_items.get_programs())
                    self.data_.update(included_items.get_data())
                    self.dstdirs_.update(included_items.dstdirs_)
            else:
                if line[0] not in string.whitespace:
                    self.reset(sl)
                else:
                    self.process(sl)

    def platform_ok(self, plat):
        return "all" == plat or self.platform_.startswith(plat) or \
               ("win32" != self.platform_ and "unix" == plat)

    def section_ok(self, section):
        for section_tag in section.split(','):
            if section_tag in self.sections_:
                return True
        return False

    def resolve_variable(self, s):
        b = ""
        while (b != s):
            b = s
            s = var_re.sub(lambda x: varsub(x, self.vardict_), b)
        return s

    def add_makefile_variables(self, vstr):
        self.makefile_variables_.extend(vstr.split())

    def add_variable(self, vstr):
        name, value = vstr.split('=', 1)
        value = re.sub(r'%(\d\d)', lambda x: chr(int(x.group(1), 16)), value)
        if ("platform" == name):
            self.platform_ = value
        self.vardict_[name.strip()] = value.strip()

    def add_sections(self, sections):
        self.sections_.extend(map(string.strip, sections.split(',')))

    def reset(self, line):
        fields = line.split(':', 2)[0:3]
        fields = [self.resolve_variable(x.strip()) for x in fields]
        self.type_, self.dstdir_, self.srcdir_ = fields
        if self.dstdir_ not in self.dstdirs_:
            self.dstdirs_[self.dstdir_] = []
        self.by_distdir_ = self.dstdirs_[self.dstdir_]
        if ("programs" == self.type_):
            self.dict_ = self.prog_
        else:
            self.dict_ = self.data_

    def process(self, line):
        if (0 == len(self.dstdirs_.keys())):
            return
        s = line.split(' ', 1)[0:2]
        dst = s[0]
        
        symlink = False
        if (len(s) > 1):
            src = s[1].strip()
            if src.startswith('->'):
                src = src[len('->')+1:].strip()
                symlink = True
        else:
            src = dst
        dst, src = [ self.resolve_variable(x) for x in dst, src ]
        item = {'srcdir': self.srcdir_,
                'src': src,
                'dstdir': self.dstdir_,
                'dst': dst }
        if symlink:
            item['symlink'] = True

        dstfp = os.path.join(self.dstdir_, dst)
        self.by_distdir_.append(item)
        self.dict_[dstfp] = item

    def get_programs(self): return self.prog_

    def get_destdirs(self): return self.dstdirs_.keys()

    def get_by_destdirs(self): return self.dstdirs_

    def get_data(self): return self.data_

    def get_variables(self): return self.vardict_

    def get_makefile_variables(self): return self.makefile_variables_

