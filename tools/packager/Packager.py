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
#!/usr/bin/env python2.0
import string, sys, os.path, os, shutil, fnmatch, re
import AppRunner, ArgParser

class AppException(Exception):
    def __init__(self, *args):
        Exception.__init__(self, *args)

class ArgException(AppException):
    def __init__(self, length, min, max, callerFunc):
        self.min = min
        self.max = max
        self.callerFunc = callerFunc
        AppException.__init__(self, min, max, callerFunc)

    def __str__(self):
        if self.max is None:
            return "Required: %s argument(s).\n%s" % (self.min,\
                  self.callerFunc.__doc__)
        elif -1 == self.max:
            return "Required: from %s arguments.\n%s" % (self.min,\
                  self.callerFunc.__doc__)
        else:
            return "Required: from %s to %s argument(s).%s\n" % \
                  (self.min, self.max, self.callerFunc.__doc__)

class WrongOption(AppException):
    def __init__(self, option, callerFunc):
        self.option = option
        self.callerFunc = callerFunc
        AppException.__init__(self, option, callerFunc)

    def __str__(self):
        return "Wrong option: %s\n%s" % (self.option, \
                                         self.callerFunc.__doc__)

class WrongSearchPath(AppException):
    def __init__(self, dir):
        self.dir = dir
        AppException.__init__(self, dir)

    def __str__(self):
        return "Wrong search path: %s specified" % (self.dir)

class CopyToDir(AppException):
    def __init__(self, dir):
        self.dir = dir
        AppException.__init__(self, dir)

    def __str__(self):
        return "Trying to copy file over dir: %s" % (self.dir)

class CopyToFile(AppException):
    def __init__(self, dir):
        self.dir = dir
        AppException.__init__(self, dir)

    def __str__(self):
        return "Trying to make dir over file: %s" % (self.dir)

class DistDirRemove(AppException):
    def __init__(self, dir, x):
        self.dir = dir
        self.x = x
        AppException.__init__(self, dir, x)

    def __str__(self):
        return "Directory: %s cant be removed: %s" % (self.dir, \
                                                      str(self.x))

class DistDirCreate(DistDirRemove):
    def __str__(self):
        return "Directory: %s cant be created: %s" % (self.dir, \
                                                      str(self.x))

class InvalidExclusion(AppException):
    def __init__(self, excl):
        self.excl = excl
        AppException.__init__(self, excl)

    def __str__(self):
        return "Invalid exclusion: %s specified." % (self.excl)

class NotDirectory(AppException):
    def __init__(self, file):
        self.file = file
        AppException.__init__(self, file)

    def __str__(self):
        return "Trying to copy several path to a single file: %s" % (self.file)

class FilesNotFound(AppException):
    def __init__(self, files, searchPath):
        self.files = files
        self.searchPath = searchPath
        AppException.__init__(self, files, searchPath)

    def __str__(self):
        return "Files: %s were not found in: %s" % (self.files, self.searchPath)

class ExportFileError(AppException):
    def __init__(self, fName, strerror):
        self.fName = fName
        self.strerror = strerror
        AppException.__init__(self, fName, strerror)

    def __str__(self):
        return "Failed to open file: %s" \
               % (self.fName)

class AppArgParser(ArgParser.Parser):
    def __init__(self, app, callerFunc, args, flags = "",
                 min = None, max = None):
        self.callerFunc = callerFunc
        self.app = app
        ArgParser.Parser.__init__(self, list = string.split(args),
                                  flags = flags, min = min, max = max)

    def do_wrongOption(self, option):
        raise WrongOption(option, self.callerFunc)

    def do_wrongLength(self, length):
        raise ArgException(length, self.min, self.max, self.callerFunc)
    
class Interface:

    cmdErr = 2

    def __init__(self):
        pass
        
    def do_exclude(self, args):
        """
        Adds exclusion fnmatch.
        
        Arguments: [name] exclusion...

        If name is '*' then it is global exclusion.
        """
        class Ap(AppArgParser):
            def do_All(self, opts, args):
                self.app.addExclusion(args)

        return Ap(self, Interface.do_exclude, args, min = 2, max = -1).run()

    def do_copy(self, args):
        """
        Copies targets to distribution:
        
        Arguments: [-x exclusion] targets... destination
        """
        class Ap(AppArgParser):
            def do_All(self, opts, args):
                exclusions = []
                path = os.curdir
                for o, a in opts:
                    if o == "-x":
                        exclusions.append(a)
                    if o == "-p":
                        path = a
                self.app.copy(exclusions, args, path)

        return Ap(self, Interface.do_copy, args, min = 2, max = -1,
                  flags = "x:p:").run()

    def do_move(self, args):
        """
        Renames file relative the distribution directory
        
        Arguments: oldName newName
        """
        class Ap(AppArgParser):
            def do_All(self, opts, args):
                self.app.move(args[0], args[1])

        return Ap(self, Interface.do_copy, args, min = 2).run()

    def do_exec(self, args):
        """
        Executes script. Gives dist dir as the first argument.
        """
        class Ap(AppArgParser):
            def do_All(self, opts, args):
                self.app.execute(args[0], args[1:])

        return Ap(self, Interface.do_exec, args, min = 1, max = -1).run()

class Impl(AppRunner.App, Interface):

    currentVer = "0.1"
    excl = "*"
    
    def __init__(self, pack_name, version, dist_dir, pack_type, src_dir, verbose):
        self.packName = pack_name
        self.version = version
        self.distDir = dist_dir
        self.srcDir = src_dir
        if sys.platform[:3] == "win":
            string.replace(version, ".", "_")
        if version:
            self.packageDir = string.join((pack_name, "-", version), "")
        else:
            self.packageDir = pack_name
        self.packType = pack_type
        self.exclusions = {Impl.excl : ""}
        self.pathCache = {}
        AppRunner.App.__init__(self, "DP> ",
                               "\nDistribution Packager v %s\n" % Impl.currentVer,
                               verbose)

    def pack(self, manifest):
        try:
            try:
                fd = open(manifest, "r")
                self.prepareDistTree()
                self.runFile(fd)
            except IOError, e:
                raise ExportFileError(manifest, e.strerror)
            self.makePackage()
        except Exception, x:
            self.outputException(x)
            return self.cmdErr

    def move(self, oldName, newName):
        destination = os.path.join(self.distDir, self.packageDir)        
        oldName = os.path.abspath(os.path.join(destination, oldName))
        newName = os.path.abspath(os.path.join(destination, newName))
        if not os.path.exists(oldName):
            raise FilesNotFound(oldName, self.distDir)
        os.renames(oldName, newName)

    def makePackage(self):
        if self.packType == "no":
            return
        if self.packType == "tgz":
            tar_file = "%s/%s.tgz" % (self.distDir, self.packageDir)
            if os.path.exists(tar_file):
                os.remove(tar_file)
            print "Tarring %s..." % tar_file 
            os.system("tar -C %s -czf %s %s" % (self.distDir,
                                                tar_file,
                                                self.packageDir))
        else:
            zip_file = "%s.zip" % (self.packageDir)
            print "Zipping %s... in %s" % (zip_file, self.distDir)
            cur_dir = os.getcwd()
            os.chdir(self.distDir)
            if os.path.exists(zip_file):
                os.remove(zip_file)
            files = os.path.join(self.packageDir, "*")
            os.system("zip -9qr %s %s" % (zip_file, files))
            os.chdir(cur_dir)
        
    def prepareDistTree(self):
        packDir = os.path.join(self.distDir, self.packageDir)
        if os.path.isdir(packDir):
            try:
                shutil.rmtree(packDir)
                print "REMOVED TREE"
            except OSError, e:
                raise DistDirRemove(packDir, str(e))
        try:
            os.makedirs(packDir)
        except OSError, e:
            raise DistDirCreate(packDir, str(e))

    def addExclusion(self, exclusion):
        print "Adding Exclusion %s" % exclusion
        self.exclusions[exclusion[0]] = exclusion[1:]
        print "Have %s" % self.exclusions

    def copy(self, exclusions, files, path):
        """
        exclusions - The list of exclusion names (not wildcards)
        files - arguments of the copy command
        path - search path where to take the copy files from
        """
        # destination is the subdirectory in the distribution tree
        destination = ""
        if len(files) > 1:
            destination = files[-1]
            files = files[:-1]
        tree_destination = destination
        destination = os.path.join(self.distDir, self.packageDir,
                                   destination)

        # Analysing exclusions
        for i in exclusions:
            if i not in self.exclusions.keys():
                raise InvalidExclusion(i)
        if Impl.excl not in exclusions:
            exclusions.append(Impl.excl) # Global exclusion is always there
        if self.srcDir:
            path = string.replace(path, "%src%", self.srcDir)            
        print "\nCopying:%s\nExcluding %s\nFrom path %s\nTo %s" % (files,
                                                                   exclusions,
                                                                   path,
                                                                   destination)
        # Find all the files in the given path
        path = os.path.abspath(path)
        if not os.path.isdir(path):
            print "Path", path, os.path.isdir(path), os.curdir
            raise WrongSearchPath(path)
        if path not in self.pathCache.keys():
            self.pathCache[path] = findall(path)
        #print "Found in %s:\n%s" % (path, self.pathCache[path])

        # Remove the exclusions:
        clean_file_list = []
        for f in self.pathCache[path]:
            to_exclude = 0
            for i in exclusions:
                for j in self.exclusions[i]:
                    if fnmatch.fnmatch(f, j):
                        to_exclude = 1
            if not to_exclude:
                clean_file_list.append(f)
        #print "Files after exclusions:\n", clean_file_list

        # Extract the specified files from given wildcards
        requested_files = {}
        for i in clean_file_list:
            for f in files:
                if fnmatch.fnmatch(i, f):
                    requested_files[i] = ""
        # If no requested files really found: error
        for f in files:
            is_match = 0
            for i in requested_files.keys():                
                if fnmatch.fnmatch(i, f):
                    is_match = 1
            if not is_match:
                raise FilesNotFound(f, path)
        if len(requested_files.keys()) == 0:
            raise FilesNotFound(files, path)

        # Check before copying if file already exist!..
        requested_files = requested_files.keys()
        #print "REQUESTED:", requested_files
        for i in requested_files:
            copy_path = os.path.join(destination, i)
            tree_path = os.path.join(tree_destination, i)
            if os.path.isdir(copy_path):
                raise CopyToDir(copy_path)
            original = os.path.join(path, i)
            print "%s -> \t\t %s" % (original, tree_path)
            dest_dir = os.path.dirname(copy_path)
            if not os.path.exists(dest_dir):
                os.makedirs(dest_dir)
            if os.path.isfile(dest_dir):
                raise CopyToFile(dest_dir)
            shutil.copyfile(original, copy_path)
            shutil.copymode(original, copy_path)

    def execute(self, script, args):
        print "NOT Executing %s with args %s" % (script, args)

    def copyToDist(self, file, destination):
        # destination i
        pass

# Taken from distutils filelist.py
def findall (dir = os.curdir):
    """Find all files under 'dir' and return the list of full filenames
    (relative to 'dir').
    """
    from stat import ST_MODE, S_ISREG, S_ISDIR, S_ISLNK

    orig_dir = os.getcwd()
    os.chdir(dir)

    list = []
    stack = [os.curdir] # was dir
    pop = stack.pop
    push = stack.append

    while stack:
        dir = pop()
        names = os.listdir(dir)

        for name in names:
            if dir != os.curdir:        # avoid the dreaded "./" syndrome
                fullname = os.path.join(dir, name)
            else:
                fullname = name

            # Avoid excess stat calls -- just one will do, thank you!
            stat = os.stat(fullname)
            mode = stat[ST_MODE]
            if S_ISREG(mode):
                list.append(fullname)
            elif S_ISDIR(mode) and not S_ISLNK(mode):
                push(fullname)

    os.chdir(orig_dir)
    return list
