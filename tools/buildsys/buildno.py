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
import sys, os
from subprocess import *

srcdir, buildno_file, buildno_h = sys.argv[1:4]

rev = '0'

cwd = os.getcwd()

try:
    if sys.platform != "win32":
        llp = "LD_LIBRARY_PATH"
        if sys.platform == "darwin":
            llp = "DYLD_LIBRARY_PATH"
        os.environ[llp] = "/usr/lib:/usr/local/lib:%s" % os.environ[llp]
    os.chdir(srcdir)
    print srcdir, os.getcwd()
    svn = Popen("svn info .", shell=True, stdout=PIPE, stderr=STDOUT, 
                close_fds=True)
    for line in svn.stdout:
        if line.startswith("Revision: "):
            rev = line.rstrip().split(' ', 2)[1].strip()
            break
            
except Exception, e:
    print >> sys.stderr, str(e)
    pass

os.chdir(cwd)

open(buildno_file, "w+").write(rev)
open(buildno_h, "w+").write(
"""
#ifndef SERNA_BUILDREV
# define SERNA_BUILDREV %s
#endif
""" % rev)

sys.exit(0)
