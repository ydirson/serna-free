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
from subprocess import *

def usage():
    print >> sys.stderr, "Usage: %s <rpm_source_spec> <rpm_spec> <source_binaries_listfile>"
    sys.exit(-1)
    
if len(sys.argv) < 4:
    usage()

var_re = re.compile(r"@(?P<name>[^@]+)@")
path_re = re.compile(r"(?P<varref>\$\{(?P<varname>[^\}]+)\})")

def expand_path(m_obj, vardict):
    varname= m_obj.group("varname")
    if varname in vardict:
        return vardict[varname]
    return ""

def replstring(m_obj, vardict):
    name = m_obj.group("name")
    if name.startswith("file "):
        path = name[len("file "):]
        nsubs = 1
        while nsubs > 0:
            path, nsubs = path_re.subn(lambda x, d=vardict: expand_path(x, d), path)
        if os.path.exists(path):
            return file(path, "r").read()
    elif name in vardict:
        return vardict[name]
    print >> sys.stderr, "%s: Cannot expand variable '%s' in file '%s'" % \
        (os.path.abspath(sys.argv[0]), m_obj.group(), os.path.abspath(sys.argv[1]))
    sys.exit(-1)

def expand(line, dict = os.environ):
    return var_re.sub(lambda x, d=dict: replstring(x, d), line)

def run(cmd, to_stdout = None):
    proc = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, 
                 stderr=PIPE, close_fds=True)
    
    if not to_stdout is None:
        proc.stdin.write(to_stdout)
        proc.stdin.close()
        
    out = proc.stdout.read()
    errors = ""
    if not proc.stderr is None:
        errors = proc.stderr.read()
    status = proc.wait()
    if 0 > status:
        print >> sys.stderr, "'%s' command exited abnormally" % cmd
        sys.exit(status)
    
    if 0 != status:
        print >> sys.stderr, "'%s' command returned %d, its stderr output = '%s'" \
            % (cmd, status, errors)
        sys.exit(status)
        
    return out
                
class RpmSpec:
    def __init__(self, rpmspec = None):
        if not rpmspec is None:
            self.read(rpmspec)
        self.header = []
        self.hdr_dict = {}
        self.body = ""
    
    def read(self, rpmspec):
        if not os.path.exists(rpmspec):
            return False
        
        rpmfile = file(rpmspec, "r")
        for rawline in rpmfile:
            line = rawline.strip()
            if 0 == len(line):
                for rawline in rpmfile:
                    self.body += expand(rawline)
                break
            name, value = line.split(':', 1)
            expanded_value = expand(value).strip()
            if name in self.hdr_dict:
                expanded_value = self.hdr_dict[name] + ' ' + expanded_value
            else:
                self.header.append(name)
            self.hdr_dict[name] = expanded_value

        return "Version" in self.hdr_dict
        
    def find_requires(self, binlistfile):
        if "Requires" in self.hdr_dict:
            return
        if not os.path.exists(binlistfile):
            print >> sys.stderr, "Binaries list file '%s' does not exist" % binlistfile
            sys.exit(-1)
            
        binlist = [ x for x in file(binlistfile, "r") ]
        if 0 == len(binlist):
            print >> sys.stderr, "%s: empty list of binary files" % os.path.abspath(binlistfile)
            sys.exit(-1)

        binaries = '\n'.join(binlist)
        reqlist = run(r'/usr/lib/rpm/find-requires', binaries).split()
        provlist = run(r'/usr/lib/rpm/find-provides', binaries).split()
	file("provlist.txt", "w+").write('\n'.join(provlist))
	file("reqlist.txt", "w+").write('\n'.join(reqlist))
        provdict = dict( [ [x,1] for x in provlist ])
        newreqlist = [ x for x in reqlist if not x in provdict ]
        if 0 == len(newreqlist):
            print >> sys.stderr, "%s: empty list of required libraries" % os.path.abspath(binlistfile)
            sys.exit(-1)
        self.hdr_dict["Requires"] = ' '.join(newreqlist)
        self.header.append("Requires")
        
    def dump(self, outfile):
        dumpfile = file(outfile, "w")
        for n in self.header:
            print >> dumpfile, "%s: %s" % (n, self.hdr_dict[n])
        print >> dumpfile, "\n", self.body

rpmspec = RpmSpec();
if rpmspec.read(sys.argv[1]):
#    rpmspec.find_requires(sys.argv[3])
    rpmspec.dump(sys.argv[2])
else:
    print >> sys.stderr, "Invalid rpm spec file '%s'" % sys.argv[1]
    sys.exit(-1)
