#!/usr/bin/python
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

# Copyright (c) 2003 Syntext, Inc. All Rights Reserved.
#
# 

import os, sys, getopt

usage_string = \
"""
Usage: %s [-h] [-v] [-u] [-b <source_root>] out

-h              This message
-v              Show program version
-u              Use unix install prefix
-s <src_root>   Use <src_root> to absolutize source paths
-d <dst_root>   Use <dst_root> to absolutize destination paths
out             Output filename

Generate MANIFEST piece for directory tree starting current directory.
"""

install_prefix = "${inst_prefix}/${serna}"
unix_install_prefix = "${inst_prefix}/serna"
source_path = "${top_srcdir}/serna/dist/"
destination_path = ''

version_string = "0.1"

skip = [sys.argv[0], "CVS", ".svn"]

def main():
    global destination_path
    print "Manifest Index Tool ver. %s Copyright (c) 2003 Syntext, Inc." % version_string
    # Check for required arguments
    if len(sys.argv) < 2:
        getUsageStderr(); return 1
    
    try:
        # Parsing command line options
        opts, args = getopt.getopt(sys.argv[1:],"vhus:d:")

        # Processing some nonessential options
        for option, argument in opts:
            if option in ("-v", ): getVersion(); return
            if option in ("-h", ): getUsage(); return
            if option in ("-u", ):
                global install_prefix
                install_prefix = unix_install_prefix
                print "Using unix install prefix: %s" % install_prefix
            if option in ("-s", ):
                global source_path
                source_path = argument
                print "Using source path: %s" % source_path
            if option in ("-d", ):
                destination_path = argument
                print "Using destination path: %s" % destination_path

        install_prefix = os.path.join(install_prefix, destination_path)
        print "Using install path: %s" % install_prefix

        # Building for all tasks required in turn
        for mft_filename in args:
            skip.append(mft_filename)
            mft_file = open(mft_filename, "w")
            mft_file.write(getDirectoryManifest())
            mft_file.close()
            print "Manifest template saved as: %s" % mft_filename

    except getopt.GetoptError, e:
        getUsageStderr()
        return 1

def getDirectoryManifest(directory='.'):
    os.dirsep = '/'
    manifest = ""

    for top, dirs, files in os.walk(directory):
        print top
        for d in dirs:
            if d in skip:
                del dirs[dirs.index(d)]
        for f in files:
            if f in skip:
                del files[files.index(f)]
        dst_dir, src_dir = [ os.path.join(x, top).replace('\\', '/').rstrip('/') \
                                 for x in install_prefix, source_path ]
        header = "data:%s:%s\n    " % (dst_dir, src_dir)
        header += "\n    ".join(files) + "\n\n"
        manifest += header

    return manifest

def getUsage():
    print usage_string % os.path.split(sys.argv[0])[1]

def getUsageStderr():
    print >> sys.stderr, usage_string % os.path.split(sys.argv[0])[1]

def getVersion():
    print version_string

if __name__ == "__main__":
    sys.exit(main())
