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

class PropIter(object):
    def __init__(self, props):
        self._child = props.firstChild()

    def __iter__(self): return self

    def next(self):
        if self._child:
            rv = self._child
            self._child = self._child.nextSibling()
            return rv
        raise StopIteration

def dump_ptree(node, indent=0):
    print ' ' * indent + "%s: '%s'" % (unicode(node.name()),
                                       unicode(node.getString()))
    child = node.firstChild()
    while child:
        dump_ptree(child, indent + 2)
        child = child.nextSibling()

class Value(object):
    def __init__(self, **kwargs):
        self.__dict__["_dict"] = kwargs.copy()

    def __contains__(self, obj):
        return self.__dict__["_dict"].__contains__(obj)

    def __getattr__(self, name):
        dct = self.__dict__["_dict"]
        if dct.has_key(name):
            return dct[name]
        elif hasattr(dct, name):
            return getattr(dct, name)
        return None

    def __setattr__(self, name, value):
        self.__dict__["_dict"][name] = value

    def __delattr__(self, name):
        del self.__dict__["_dict"][name]

    def attrs(self):
        return self.__dict__["_dict"].copy()

    def update(self, mapping):
        self.__dict__["_dict"].update(mapping)

    def dump(self, indent=0):
        for k, v in self.attrs().iteritems():
            if hasattr(v, 'dump'):
                print "%s%s:" % (' ' * indent, k)
                v.dump(indent + 2)
            else:
                print "%s%s: '%s'" % (' ' * indent, k, str(v))

    __getitem__ = __getattr__
    __setitem__ = __setattr__
    __delitem__ = __delattr__

class PublishException(Exception):
    def __init__(self, errorString):
        self._errorString = errorString

    def __str__(self):
        return self._errorString

    def getErrorString(self):
        return self._errorString

class Creator(Value):
    def __init__(self, name, **kwargs):
        Value.__init__(self, **kwargs)
        self.name = name

    def getTag(self):
        if self.tag:
            return self._tag
        return self.getName()

    def getName(self):
        return self.name

class Factory(object):
    def __init__(self):
        self._creators = {}

    def __iter__(self):
        return self._creators.itervalues()

    def addCreator(self, creator, tag=None):
        try:
            if not tag:
                tag = creator.getTag()
            if tag:
                self._creators[tag] = creator
                return True
        except:
            pass
        return False

    def getCreator(self, tag):
        try:
            if tag in self._creators:
                return self._creators[tag]
        except:
            pass
        return None

    def make(self, tag, **kwargs):
        if hasattr(tag, 'getTag'):
            tag = tag.getTag()

        creator = self._creators.get(tag, None)
        if creator:
            kwargs['tag'] = tag
            if hasattr(creator, 'make'):
                return creator.make(**kwargs)
            if hasattr(creator, '__call__'):
                return creator(**kwargs)

        return None

    def dump(self):
        print "Factory:"
        for k, v in self._creators.iteritems():
            if hasattr(v, 'dump'):
                print "  For tag '%s':" % str(k)
                v.dump(4)

from types import ListType, TupleType, DictType

def isDict(arg):
    return isinstance(arg, DictType)

def isSequence(arg):
    return isinstance(arg, ListType) or isinstance(arg, TupleType)

def isString(arg):
    return isinstance(arg, basestring)

import os

def get_tmpfile_name(uri=None, tmpExt=None, baseDir=None):
    from tempfile import mkstemp
    if tmpExt is None:
        tmpExt = '.tmp'
    try:
        if uri or baseDir:
            if baseDir:
                tempDir = baseDir
            else:
                tempDir = os.path.dirname(os.path.abspath(uri))
            tmpFd, tmpName = mkstemp(suffix=tmpExt, dir=tempDir)
        else:
            tmpFd, tmpName = mkstemp(suffix=tmpExt)
    except:
        tmpFd, tmpFile = mkstemp(suffix=tmpExt)

    os.close(tmpFd)
    return tmpName

def set_java_home(path):
    os.environ["JAVA_HOME"] = path

def quote(s):
    return unicode('"%s"' % s.strip('"'))

def find_file_in_path(fname, paths):
    isWin32 = ('win32' == sys.platform)
    for path in paths:
        tryFile = os.path.join(path, fname)
        if os.path.exists(tryFile):
            if not isWin32 and not os.access(tryFile, os.X_OK):
                continue
            return tryFile
    return None

def find_path_with_ext(fname, paths, exts):
    name, ext = os.path.splitext(fname)
    if ext in exts:
        return find_file_in_path(fname, paths)

    for ext in exts:
        fname = find_file_in_path(name + ext, paths)
        if fname:
            return fname

    return None

def find_script(fname, localVars=None):
    isWin32 = 'win32' == sys.platform

    fname = fname.strip('"')

    if '$' in fname:
        if localVars is None:
            localVars = PropertyNode()
        resolvedFname = SernaConfig.resolveResource('', '', fname, localVars)
        if resolvedFname:
            fname = unicode(resolvedFname)

    fname = os.path.normpath(fname)
    paths = []

    if os.path.isabs(fname):
        if not os.path.exists(fname) or isWin32:
            dirname, basename = os.path.split(fname)
            paths = [dirname]
            fname = basename
    else:
        scriptsDir = os.path.join(os.path.dirname(__file__), 'scripts')
        tryFile = os.path.join(scriptsDir, fname)
        if os.path.exists(tryFile):
            fname = tryFile
        else:
            if os.path.dirname(fname):
                return fname

            paths = os.environ.get('PATH', '').split(os.pathsep)
            paths.insert(0, scriptsDir)

    if not os.path.exists(fname) or paths:
        if isWin32:
            pathExts = ['.bat', '.cmd', '.exe']
        else:
            pathExts = ['', '.sh']

        found = find_path_with_ext(fname, paths, pathExts)
        if found:
            fname = found

    if not isWin32 and not os.access(fname, os.X_OK):
        return ''

    return os.path.normpath(fname)

from PyQt4          import  QtCore
from PyQt4.QtCore   import  QObject, QProcess, QFileInfo, QIODevice, \
                            QStringList
import locale, codecs, os, sys

def augment_value(value, prop):
    to_ascii = codecs.getencoder('ascii')
    for pn in PropIter(prop):
        if pn.firstChild():
            continue
        name, length = to_ascii(unicode(pn.name()))
        value[name] = unicode(pn.getString())

def dict2qlst(env):
    qlst = QStringList()
    for k, v in env.iteritems():
        qlst << "%s=%s" % (k, v)
    return qlst

class ScriptRunner(QObject):
    def __init__(self, caller, parent=None, env=None, addEnv=None):
        QObject.__init__(self, parent)
        self._proc = None
        self._caller = caller
        self._decode = None
        if env is None:
            env = os.environ.copy()
        self._env = env
        if addEnv:
            self._env.update(addEnv)

    def stop(self, kill=False):
        if self._proc is None:
            return

        if kill:
            self._proc.kill()
        else:
            self._proc.terminate()

    def run(self, scriptFile, args=None, env=None, wd=None):
        if self._proc:
            return

        self._proc = QProcess(self)

        if wd:
            self._proc.setWorkingDirectory(wd)

        if not env is None:
            self._env.update(env)
        envList = dict2qlst(self._env)

        if args is None:
            args = []

        script = quote(scriptFile)
        if not os.path.exists(script):
            script = find_script(script)

        if not script or not os.path.exists(script):
            raise PublishException("Script '%s' cannot be found" % script)

        self._caller.handleScriptOutput("%s %s" % (scriptFile,  ' '.join(args)))

        self._proc.setEnvironment(envList)
        self._proc.setProcessChannelMode(QProcess.MergedChannels)
        QObject.connect(self._proc, QtCore.SIGNAL("readyReadStandardOutput()"),
                        self.readScriptOutput)
        QObject.connect(self._proc, QtCore.SIGNAL("finished(int, " \
                                                  "QProcess::ExitStatus)"),
                        self.scriptFinished)

        self._proc.start(script, args, QIODevice.ReadOnly)

    def scriptFinished(self, exitCode, exitStatus):
        if self._caller:
            self.readScriptOutput()
            self._caller.scriptFinished(exitCode,
                                        exitStatus == QProcess.Crashed)
        self._proc = None

    def readScriptOutput(self):
        if self._proc is None:
            return

        qba = self._proc.readAllStandardOutput()
        if self._caller and qba.length():
            if not self._decode:
                lang, deflocale = locale.getdefaultlocale()
                self._decode = codecs.getdecoder(deflocale)
            data, datalen = self._decode(qba.data())
            self._caller.handleScriptOutput(data)
