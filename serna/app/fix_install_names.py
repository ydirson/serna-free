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

import sys, os

def shell(cmd):
    cout = os.popen('/bin/sh -c "%s"' % cmd, "r")
    output = [ x.strip() for x in cout.readlines() ]
    rc = cout.close()
    if rc is None: rc = 0
    return rc, output

def is_exe(path):
    testcmd = "/usr/bin/file -L %s | /usr/bin/grep -q 'Mach-O executable'" % path
    return os.access(path, os.X_OK) and 0 == shell(testcmd)[0]

def is_dylib(path):
    testcmd = "/usr/bin/file -L %s | /usr/bin/egrep -q 'Mach-O.*(shared library|bundle)'" % path
    return (path.endswith(".dylib") or path.endswith(".so")) and 0 == shell(testcmd)[0]

def get_libdict(path, is_exe=False):
    ld = {}
    cmd = "/usr/bin/otool -L %s" % path
    rc, output = shell(cmd)
    mybase = os.path.basename(path)
    if 0 == rc:
        for line in output[1:]:
            if line.startswith("/System"):
                continue
            libpath, rest = line.strip().split(None, 1)
            libdir, lib = os.path.split(libpath)
            if 0 == len(libdir) and lib == mybase:
                continue
            ld.setdefault(lib, []).append(libdir)
    else:
        print >> sys.stderr, "command '%s' failed with %d" % (cmd, rc)
        sys.exit(rc)
        
    return ld

exe_by_path = {}
dylib_by_path = {}
dylib_by_id = {}

top_exe_path = ''

for top, dirs, names in os.walk(sys.argv[1]):
    for name in names:
        path = os.path.join(top, name)
        if is_exe(path):
            if name == sys.argv[2]:
                top_exe_path = top
            exe_by_path[path] = get_libdict(path, True)
        elif is_dylib(path):
            dylib_by_path[path] = get_libdict(path)
            libpath, lib = os.path.split(path)
            dylib_by_id[lib] = libpath

for bin, libdict in exe_by_path.items() + dylib_by_path.items():
    exe_path = top_exe_path
    if bin in exe_by_path and top_exe_path != os.path.dirname(bin):
        exe_path = os.path.dirname(bin)
    for lib, libpaths in libdict.iteritems():
        for libpath in libpaths:
            if lib in dylib_by_id and not libpath.startswith('@'):
                rel_path = os.path.commonprefix((dylib_by_id[lib], exe_path))
                rp_len = len(rel_path)
                tail = exe_path[rp_len:]
                up_levels = tail.count(os.sep)
                if len(tail):
                    up_levels += 1
                dylib_path_tail = dylib_by_id[lib][rp_len:].lstrip(os.sep)
                linkpath = os.path.join("@executable_path", os.sep.join([".."] * up_levels), dylib_path_tail)
                cmd = "/usr/bin/install_name_tool -change %s %s %s" % \
                      (os.path.join(libpath, lib), "%s/%s" % (linkpath, lib), bin)
                print cmd
                rc = os.system(cmd)
                if rc:
                    print >> sys.stderr, "command '%s' failed with %d" % (cmd, rc)
                    sys.exit(rc)
