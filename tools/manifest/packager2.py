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

import sys, string, getopt, os, shutil, manifest

class NotFoundError(Exception):
    def __init__(self, src):  self.file_ = src
    def file(self): return self.file_

def usage():
    print >> sys.stderr, """
usage: %s -m <manifest> -n <package_name> {-v name=value}* -V <version> -d <distdir> [-N]
-m filename        Manifest file name
-n name            Package name
-V version         Package version
-v variable=value  List of variables
-d dir             Dist directory (dist by default)
-N                 Do not pack tgz""" % (sys.argv[0])
    sys.exit(1)
    
def mkdirp(dirs):
    for d in dirs:
        try: os.makedirs(d, 0755)
        except: pass
        
def copy_items(pairs, mode, update=False):
    dstfiles = []
    for dst, src in pairs:
        srcfp = os.path.join(src['srcdir'], src['src'])
        dstfp = os.path.join(src['dstdir'], src['dst'])
        if 'symlink' in src:
            os.unlink(dstfp)
            os.symlink(src['src'], dstfp)
        else:
            shutil.copy(srcfp, dstfp)
            os.chmod(dstfp, mode)
    return dstfiles
        
def is_source_present(items):
    for src in items.get_programs().values() + items.get_data().values():
        srcfp = os.path.join(src['srcdir'], src['src'])
        if (not os.access(srcfp, os.R_OK)):
            raise NotFoundError(src)

def copy(items, var, update=False):
    # check sources existence before doing any real copying
    is_source_present(items)
    dstdir = var["distdir"]
    if (os.path.exists(dstdir) and not update):
        shutil.rmtree(dstdir, 1)

    mkdirp(items.get_destdirs())

    progs = items.get_programs()
    dstfiles = copy_items(progs.items(), 0755, update)
    
    data = items.get_data()
    dstfiles += copy_items(data.items(), 0644, update)
    return dstfiles
        
def pack(var):
    if (not var["pack"]):
        return
    dstdir, pkg = var["distdir"], "%s.tgz" % (var["package"])
    if (os.path.exists(pkg)):
        os.unlink(pkg)
    cwd = os.getcwd()
    os.chdir(dstdir)
    cmd = "tar -czf %s --exclude %s *" % (pkg, pkg)
    print cmd
    os.system(cmd)
    os.chdir(cwd)
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "m:n:v:V:d:Ns:up:")
    if (len(args) > 0): usage()
    update = False
    destination_manifest = ''
    
    var = {"manifest" : None, 
           "package" : None, 
           "version" : None, 
           "distdir" : "dist",
           "pack"    : 1}
           
    items = manifest.Items()
    for o, a in opts:
        a = a.strip()
        if ("-m" == o and var["manifest"] is None): 
            var["manifest"] = a
        elif ("-V" == o and var["version"] is None):
            var["version"] = a
        elif ("-n" == o and var["package"] is None):
            var["package"] = a
        elif ("-N" == o):
            var["pack"] = 0
        elif ("-d" == o):
            var["distdir"] = a
        elif ("-s" == o):
            items.add_sections(a)
        elif ("-v" == o):
            items.add_variable(a)
        elif ("-u" == o):
            update = True
        elif ("-p" == o):
            destination_manifest = a
        else:
            usage()
            
    for k in var.keys():
        if (var[k] is None):
            print >> sys.stderr, "You must specify", k, "variable"
            usage()

    items.add_variable("inst_prefix=" + var["distdir"])
    items.read(var["manifest"])
    dstfiles = copy(items, var, update)
    if destination_manifest:
        if dstfiles:
            dstfiles.sort()
            dst_manifest_fobj = open(destination_manifest, "wb")
            print >> dst_manifest_fobj, '\n'.join(dstfiles)
        else:
            print >> sys.stderr, "List of copied files is empty"
            sys.exit(-1)

    pack(var)
    sys.exit(0)

except getopt.GetoptError:
    usage()
    pass
except NotFoundError, e:
    print >> sys.stderr, "Source file %s not found" % (e.file())
    pass

sys.exit(1)
