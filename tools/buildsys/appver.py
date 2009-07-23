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
import sys, time, re, os

def get_appver(ver_file):
    appver = None
    try:
        for line in open(ver_file).readlines():
            if line[0] != '#':
                appver = line.strip()
                break
    except:
        print >> sys.stderr, "Can't read appver from file '%s'" % ver_file
        sys.exit(1)

    if appver is None:
        print >> sys.stderr, "File '%s' doesn't contain a valid appver" % ver_file
        sys.exit(1)

    mobj = re.match("^([^\.]+)\.([^\.]+)\.(.*)$", appver)
    if mobj is None:
        print >> sys.stderr, "Invalid appver '%s' in file '%s'" % (appver, ver_file)
        sys.exit(1)
    return mobj.group(1, 2, 3), mobj.group(0)

def gen_info(out_file, ver_file):
    appver, appverstr = get_appver(ver_file)
    ver_dict = {}
    for k,v in zip(("MAJOR", "MINOR", "PATCH"), appver):
        numvalkey = "APP_VER_" + k
        strvalkey = "APP_VER_" + k + "_TAG"
        ver_dict[strvalkey] = v
        try:
            ver_dict[numvalkey] = int(v)
        except:
            ver_dict[numvalkey] = 0

    ver_dict["APP_VERSION"] = '.'.join([ver_dict["APP_VER_" + x + "_TAG"] \
                                        for x in "MAJOR", "MINOR", "PATCH"])
    ver_dict["VERSION_ID"] = '.'.join([ver_dict["APP_VER_" + x + "_TAG"] \
                                        for x in "MAJOR", "MINOR"])
    bld_date = time.strftime("%Y%m%d", time.localtime())
    ver_dict["APP_BUILD_DATE"] = bld_date
    bld_stime = time.strptime(bld_date, "%Y%m%d")
    epoch_time = int(time.mktime(bld_stime))
    ver_dict["APP_BUILD_TIME"] = epoch_time

    if os.path.exists(out_file) and os.path.isfile(out_file):
        prev_dict = read_info(out_file)
        equal = True
        for k in ver_dict.keys():
            if not k in prev_dict or str(ver_dict[k]) != prev_dict[k]:
                equal = False
                break
        if equal:
            return
    out_fobj = file(out_file, "w+")
    for k, v in ver_dict.iteritems():
        print >> out_fobj, k, '=', v

def is_older(target, dep):
    if not os.path.exists(target):
        return True
    t_mtime, d_mtime = [ os.stat(x).st_mtime for x in target, dep ]
    if d_mtime > t_mtime:
        return True
    return False

def gen_sources(basename, ver_dict, module):
    export, expinc = "", ""
    if 'win32' == sys.platform and 0 < len(module):
        export = module.upper() + "_EXPIMP "
        expinc = '\n#include "' + module + '/' + module + '_defs.h"\n'
    ver_dict["expinc"] = expinc
    ver_dict["export"] = export

    hpp_template = """\
#ifndef APPVER_H_
#define APPVER_H_
#ifndef RC_INVOKED
%(expinc)s
#include <time.h>

%(export)sextern const char APP_VERSION[];
%(export)sextern const char APP_BUILD_DATE[];

#define SERNA_VERSION_ID "%(VERSION_ID)s"
%(RELEASE_TYPE)s
#define APP_VER_MAJOR_TAG "%(APP_VER_MAJOR_TAG)s"
#define APP_VER_MINOR_TAG "%(APP_VER_MINOR_TAG)s"
#define APP_VER_PATCH_TAG "%(APP_VER_PATCH_TAG)s"

const time_t APP_BUILD_TIME = %(APP_BUILD_TIME)s;

const unsigned int APP_VER_MAJOR = %(APP_VER_MAJOR)s;
const unsigned int APP_VER_MINOR = %(APP_VER_MINOR)s;
const unsigned int APP_VER_PATCH = %(APP_VER_PATCH)s;

#endif // RC_INVOKED
#endif // APPVER_H_
"""
    cpp_template = """\
#include "appver.hpp"%(expinc)s

%(export)sextern const char APP_VERSION[]     = "%(APP_VERSION)s";
%(export)sextern const char APP_BUILD_DATE[]   = "%(APP_BUILD_DATE)s";
"""
    ver_list = [ int(ver_dict["APP_VER_" + x]) for x in
                 "MAJOR", "MINOR", "PATCH" ]
    ver_dict["RELEASE_TYPE"] = "\n#define SERNA_RELEASE 0x%02d%03d%03d\n" % \
        (ver_list[0], ver_list[1], ver_list[2])

    beta_tag = ver_dict["APP_VER_MINOR_TAG"]
    if (not re.match(r"^\d+$", beta_tag)):
        if 'b' in beta_tag or 'a' in beta_tag:
            ver_dict["RELEASE_TYPE"] += '\n#define SERNA_BETA_ID "%s"\n' % beta_tag
        elif "rc" in beta_tag:
            ver_dict["RELEASE_TYPE"] += "\n#define SERNA_RELEASE_CANDIDATE %s\n" % beta_tag

    hpp_contents = hpp_template % ver_dict
    open(basename + ".hpp", "w+").write(hpp_template % ver_dict)
    open(basename + ".cpp", "w+").write(cpp_template % ver_dict)

def read_info(ver_file):
    ver_dict = {}
    for line in open(ver_file, "r"):
        line = line.strip()
        k, v = line.split(' = ', 1)
        ver_dict[k] = v
    return ver_dict

def usage():
    print >> sys.stderr,"\nUsage: appver.py <gen_file> ((<ver_file> <module>) | <info_file>)"
    sys.exit(1)

if len(sys.argv) < 2:
    usage()

gen_file, ver_file = sys.argv[1:3]
basename, ext = os.path.splitext(gen_file)
module = ""

if ".txt" != ext:
    if len(sys.argv) < 3:
        usage()
    module = sys.argv[3]
    cpp, hpp = [ basename + x for x in ".cpp", ".hpp" ]
    if is_older(cpp, ver_file) or is_older(hpp, ver_file):
        gen_sources(basename, read_info(ver_file), module)
else:
    gen_info(gen_file, ver_file)
