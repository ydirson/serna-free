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
import os, sys, string, getopt
import Packager

usage_string = \
"""
Usage: %s -m filename -n name -v version [-t type] [-d dir] [-h] [-e]

-m filename  Manifest file name
-n name      Package name
-v version   Package version
-d dir       Dist directory (dist by default)
-t type      Package type [tgz] [zip] [no] (autodetected, broken in Win)
-s srcdir    Source tree (may be used in copy with -p)
-h           This message
-e           Show errors verbosly (for debugging)

"""

def usage():
    print >> sys.stderr, usage_string % os.path.split(sys.argv[0])[1]

flags = "m:n:v:d:t:hes:"

def main():
    if len(sys.argv) > 1:
        try:
            opts, args = getopt.getopt(sys.argv[1:], flags)
        except getopt.GetoptError:
            # print help information and exit:
            usage()
            return 3
        
        file_name = None
        pack_name = None
        version = None
        dist_dir = "dist"
        src_dir = None
        if sys.platform[:3] == "win":
            pack_type = "zip"
        else:
            pack_type = "tgz"
        verbose = None

        for o, a in opts:
            if o in ("-m", ):
                file_name = a                
            if o in ("-n", ):
                pack_name = a
            if o in ("-v", ):
                version = a                
            if o in ("-d", ):
                dist_dir = a
            if o in ("-t", ):
                pack_type = a
            if o in ("-s", ):
                src_dir = a                
            if o in ("-h", ):
                usage()            
            if o in ("-e", ):
                verbose = 1

        if file_name == None or pack_name == None or \
            dist_dir == None or pack_type == None:
            usage()
            return 1

        if pack_type not in ["tgz", "zip"]:
            print "Packager must be tgz or zip"
            return 1

        app = Packager.Impl(pack_name, version, dist_dir, pack_type, src_dir, verbose)
        return app.pack(file_name)
    else:
        usage()
        return 0
    
if __name__ == "__main__":
    res = main()
    sys.exit(res)

