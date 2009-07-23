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
#!/usr/bin/env python

import os, sys, popen2, re
import pysvn
client = pysvn.Client()

usage  = """
Usage: %s URL

   Extract all the *previous* revisions of the given subversion URL to files
   basename.X where X is the revision number of the URL.""" % os.path.basename(sys.argv[0])

revRegex = re.compile(r'r(?P<rev>\d+).*')

def main(args):
    if len(args) == 1:
        print usage
        return 1

    for path in args[1:]:
        entry = client.info(path)
        if entry == None:
            print "Entry: ", path, " is not under SVN control"
            continue
        url = entry.url
        print "URL:", url

        # The following portion is a workaround some pysvn bug
        # that does not want to fetch old revisions of PNG files
        revisions = []
        f = popen2.popen2("svn log %s" % path)
        for l in f[0]:
            m = revRegex.match(l)
            if m:
                revisions.append(pysvn.Revision(pysvn.opt_revision_kind.number,
                                                int(m.group("rev"))))

        # This single line could work if not the workaround behind
        #revisions = [i["revision"] for i in client.log(url)]

        if len(revisions) == 0:
            print "File has no revisions"
            return 1

        print "Previous Revisions:", len(revisions) - 1
        url_basename = os.path.basename(url)

        for r in revisions[:-1]:
            filename = "r_" + str(r.number) + "_" + url_basename
            print "Writing", filename
            print "REV", client.update(url_basename, False, r).number
            if os.path.exists(url_basename):
                os.rename(url_basename, filename)
        client.update(url_basename, False)

    
if __name__ == '__main__':
    sys.exit(main(sys.argv))
