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

import sys, string, getopt, os, re, manifest, shutil
import subprocess, py_compile

def usage():
    print >> sys.stderr, "usage: " + sys.argv[0],
    print >> sys.stderr, "-t {rpm|iss} [-v name=value] <manifest> [<outfile>]"
    sys.exit(1)

env_re = re.compile(r"\$\{(?P<name>[^\}]+)\}")

def expand_env(srcname):
    return env_re.sub(lambda x: os.environ.get(x.group("name"), ""), srcname)

def fixmacro(x):
    return x.replace('{', '(').replace('}', ')')
    
def strip_flags():
    strip_flags = ''
    if sys.platform.startswith("linux"):
        strip_flags = "--strip-unneeded"
    elif sys.platform.startswith("darwin"):
        strip_flags = "-S -x"
    return strip_flags
    
def print_strip_cmd(srcname, outfile):
    if "win32" == sys.platform[:5]:
        return

    fullpath = expand_env(srcname)
    if not os.path.exists(fullpath):
        return

    while os.path.islink(fullpath):
        fullpath = os.path.join(os.path.dirname(fullpath), os.readlink(fullpath))
    fullpath = os.path.normpath(fullpath)

    if not os.path.exists(fullpath):
        return
        
    cmd = "file " + fullpath
    filecmd = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, 
                               stderr=subprocess.STDOUT, close_fds=True)
    res = filecmd.stdout.read()
    
    if sys.platform.startswith("linux"):
        if -1 == res.find("ELF") or -1 == res.find("not stripped"):
            return
    elif sys.platform.startswith("sunos"):
        if -1 == res.find("ELF") or -1 == res.find("not stripped"):
            return
    elif sys.platform.startswith("darwin"):
        if -1 == res.find("Mach-O") or -1 == res.find("shared library"):
            return
        
    strip_cmd = "\tstrip $(STRIP_FLAGS) $@"
    print >> outfile, strip_cmd

def update_sources(srclistfile, sources):
    sources.sort()
    if os.path.isfile(srclistfile):
        oldsources = [x.strip() for x in file(srclistfile, "r")]
        oldsources.sort()
        if oldsources == sources:
            return
        
    file(srclistfile, "w+").write('\n'.join(sources))
    
isWin32 = sys.platform.startswith('win32')
    
def print_install(outfile, item, is_prog=False):
    srcfile, dstfile = [ item[x] for x in 'src', 'dst' ]
    srcfp, dstfp = [ os.path.join(item[x[0]], x[1]) for x in ('srcdir', srcfile), 
                                                             ('dstdir', dstfile) ]
    if isWin32:
        srcfp, dstfp = [ os.path.normpath(fixmacro(x)) \
                         for x in (srcfp, dstfp) ]
    mode = '644'
    if is_prog:
        mode = '755'
    if 'symlink' in item and not isWin32:
        if os.path.isabs(srcfile) or srcfile.startswith('$'):
            print >> outfile, "%s: %s" % (dstfp, srcfp)
        else:
            srcfile = os.path.join(item['dstdir'], srcfile)
            srcfile = os.path.normpath(srcfile)
            print >> outfile, "%s: %s" % (dstfp, srcfile)
            srcfile = item['src']
        print >> outfile, "\trm -f %s" % dstfp
        print >> outfile, "\tln -s %s %s" % (srcfile, dstfp)
    else:
        tmpdst = dstfp.lower()
        if tmpdst.endswith(".pyc") or tmpdst.endswith(".pyo"):
            pysource, tmpdst = srcfp[:-1], dstfp[:-1]
            print >> outfile, "%s: %s" % (dstfp, pysource)
            srcpath = os.path.basename(pysource)
            print >> outfile, "\t%s -c 'import py_compile; py_compile.compile(%s)'" % \
                              (sys.executable, '"%s", "%s", "%s", %s' % \
                                               (pysource, dstfp, srcpath, "True"))
            if not isWin32:
                print >> outfile, "\tchmod %s $@" % mode
        else:
            print >> outfile, "%s: %s" % (dstfp, srcfp)
            if not isWin32:
                print >> outfile, "\tcp $^ $@; chmod %s $@" % mode
                if is_prog:
                    print_strip_cmd(srcfp, outfile)
            else:
                print >> outfile, "\tcopy /b $** $@"
    print >> outfile
    return srcfp

def dump_makefile(items, outfile):
    prolog = ''
    for v in items.get_makefile_variables():
        prolog += "%s\t\t?= %s\n" % (v, os.environ[v])
    prolog += "rpm_prefix              ?= ${inst_prefix}\n\n"
    prolog += "STRIP_FLAGS ?= %s\n" % strip_flags()
    prolog += "all: all-default\n"
    if isWin32:
        prolog = prolog.replace('?=', '=')
    print >> outfile, prolog
    all = []
    sources = []
    progs = items.get_programs()
    for dst, src in progs.items():
        all.append(dst)
        srcfp = print_install(outfile, src, True)
        sources.append(expand_env(srcfp))

    data = items.get_data()
    for dst, src in data.items():
        all.append(dst)
        srcfp = print_install(outfile, src)
        sources.append(expand_env(srcfp))

    cwd = os.path.abspath(os.path.curdir) + '/'
    dirlist = items.get_destdirs()
    dirlist.sort()
    all.sort()
    if isWin32:
        dirlist = [ os.path.normpath(fixmacro(x)) for x in dirlist ]
        all = [ os.path.normpath(fixmacro(x)) for x in all ]
        makedirs = "\n\tfor %%D in ($(_DESTDIRS)) do if not exist %%D md %%D"
    else:
        makedirs = "$(_DESTDIRS)"
#        makedirs = "for d in $@; do [ -d $$d ] || mkdir -p $$d; done"
    print >> outfile, "_ALL      = \t%s\n" % (' \\\n\t\t'.join(all))
    print >> outfile, "_DESTDIRS = \t%s\n" % (' \\\n\t\t'.join(dirlist))
    print >> outfile, """
all-default: install

install: makedirs $(_ALL)

makedirs: %s

$(_DESTDIRS):
	-for d in $@; do [ -d $$d ] || mkdir -p $$d; done

.PHONY: all install all-default makedirs
""" % makedirs
    return dirlist, all, sources

def dump_rpm_makefile(items, outfile):
    dirlist, all, sources = dump_makefile(items, outfile)

    cwd = os.path.abspath(os.path.curdir) + '/'
    mft = file("MANIFEST.rpm", "w+")
    mft.write('%dir '+'\n%dir '.join(dirlist)+'\n')
    mft.write('\n'.join(all))
    update_sources("MANIFEST.sources", sources)
    print >> outfile, """
MFT_DIR =  %s

all-default: rpm-install

rpm-install: install $(MFT_DIR)MANIFEST

$(MFT_DIR)MANIFEST: $(MFT_DIR)MANIFEST.rpm
	sed -e 's@\($${inst_prefix}\)@/${rpm_prefix}@' $^ > $@
""" % cwd

repls = {}    
iss_dst_re = re.compile(r"^\{[^\}]+\}(?P<dstsfx>.*)$")

def dump_iss_script(items, outfile):
    required = ("lic_file", "app_ver", "version_id", "top_builddir", "top_srcdir", \
                "inst_prefix", "THIRD_DIR", "iss_template", "serna")
    vdict = items.get_variables()
    for v in required:
        if (not v in vdict):
            print >> sys.stderr, "Variable %s is not set" % v
            sys.exit(2)
    script = open(vdict["iss_template"]).read()
    for pat, substKey in repls.iteritems():
        script = script.replace("@%s@" % pat, vdict[substKey])

    print >> outfile, script
    print >> outfile, "[Files]"
    progs = items.get_programs()
    data = items.get_data()
    for dst, src in progs.items() + data.items():
        if 'symlink' in src:
            continue
        m_obj = iss_dst_re.match(dst)
        if m_obj is None:
            continue
        srcfp = os.path.join(src['srcdir'], src['src'])
        print >> sys.stderr, dst, srcfp
        tmpsrc = "dist" + m_obj.group("dstsfx")
        tmpsrc_dir = os.path.split(tmpsrc)[0]
        dstdir = os.path.normpath(os.path.split(dst)[0])
        if not os.path.exists(tmpsrc_dir):
            os.makedirs(tmpsrc_dir, 0755)
        srcfp, tmpsrc, dstdir = [x.replace('/', os.sep) for x in srcfp, tmpsrc, dstdir]
        srcfp, tmpsrc = [os.path.normpath(x) for x in srcfp, tmpsrc]
        pysource = tmpsrc.lower()
        if pysource.endswith(".pyc") or pysource.endswith(".pyo"):
            pysource = srcfp[:-1]
            py_compile.compile(pysource, tmpsrc, os.path.basename(pysource), True)
        else:
            shutil.copyfile(srcfp, tmpsrc)
        print >> outfile, 'Source: "%s"; Destdir : "%s"; Flags: ignoreversion' % (tmpsrc, dstdir)

def dump_manifest(items, outfile):
    progs = items.get_programs()
    data = items.get_data()
    for dst, src in progs.items() + data.items():
        print >> outfile, expand_env(dst)

dumpers = { 'iss' : dump_iss_script, \
            'rpm' : dump_rpm_makefile, \
            'make' : dump_makefile, \
            'mft' : dump_manifest }

try:
    opts, args = getopt.getopt(sys.argv[1:], "t:v:s:e:m:r:")
    arglen = len(args)
    if (not arglen in (1, 2)): usage()

    items = manifest.Items()
    dump_manifest_file = None
    mft_type = None
    for o, a in opts:
        if ('-t' == o and mft_type is None):
            mft_type = a.strip()
        elif ('-v' == o):
            items.add_variable(a)
        elif ('-e' == o):
            items.add_makefile_variables(a)
        elif ('-s' == o):
            items.add_sections(a)
        elif ('-m' == o):
            dump_manifest_file = a
        elif ('-r' == o and '=' in a):
            k, v = a.split('=')
            repls[k.strip()] = v.strip()
        else:
            usage()

    if (not mft_type in dumpers): usage()
    items.read(args[0])
    if (arglen == 1): outfile = sys.stdout
    else: outfile = open(args[1], "w+")

    dumpers[mft_type](items, outfile)
    if dump_manifest_file:
        dumpers['mft'](items, open(dump_manifest_file, "w+"))

except getopt.GetoptError:
    usage()
#except Exception, e:
#    print >> sys.stderr, str(e)
#    sys.exit(1)

sys.exit(0)
