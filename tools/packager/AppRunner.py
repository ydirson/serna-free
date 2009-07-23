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

import sys, string, cmd, StringIO

# readline module is not available in some installations (e.g. under windows)
# If readline is not imported here, cmd simply will not use it.
try:
    import readline
except:
    pass

class App(cmd.Cmd):
    def __init__(self, prompt, intro, verbose = 1):
        self.prompt = prompt 
        self.intro = intro
        self.verbose = verbose

    def cmdloop(self, intro=None):
        try:
            cmd.Cmd.cmdloop(self, intro)
        except KeyboardInterrupt:
            print "\n"
            
    def onecmd(self, line, errReturn = 0, isRethrow = 0):
        try:
            return cmd.Cmd.onecmd(self, line)
        except Exception, x:
            if isRethrow:
                raise
            if isinstance(x, StandardError):
                print >> sys.stderr, "WARNING !!!: Python exception:"
            self.outputException(x)
            return errReturn

    def outputException(self, x):
        """
        Returns parsable error message in the form:

        E: ExceptClass: Description

        Where description is the string, returned by str(x)
        """
        cl = x.__class__
        out_str = "E: %s.%s: %s\n" % (cl.__module__, cl.__name__, str(x))
        print >> sys.stderr, out_str
        if self.verbose:
            import traceback
            if hasattr(x, "__class__"):
                print >> sys.stderr, x.__class__.__name__
            print >> sys.stderr, "Verbose Error:"
            traceback.print_exc(file = sys.stderr)            

    def postcmd(self, stop, line):
        if stop == self.cmdErr:
            return 0  ## Do not exit on ecr command errors
        return stop

    def emptyline(self):
        pass

    def do_EOF(self, arg):
        """
        Press CTRL-D to quit application.
        """
        print "\n"
        return 1

    def default(self, line):
        s = "\nUnrecognized command: %s\n" % line
        print s
        return Packager.Interface.cmdErr
        return 0

    def do_help(self, line):
        """
        Prints help on specific command. If no arguments given prints all
        available commands.

        Arguments: [command]
        """
        return cmd.Cmd.do_help(self, line)

    do_h = do_help

    def do_quit(self, line):
        """
        Quit application.
        """
        return 1

    do_q = do_quit

    def readStdin(self):
        """
        Reads and executes commands from stdin.
        """
        return self.runFile(sys.stdin)

    def do_rbatch(self, args):
        """
        Run commands from given batch file.

        Arguments: filename
        """
        class Ap(App.AppArgParser):
            def do_1(self, opts, args):
                try:
                    fd = open(args[0], "r")
                except IOError, e:
                    raise App.ExportFileError(args[0], e.strerror)
                return self.app.runFile(fd)
        return Ap(self, App.do_rbatch, args, min = 1).run()

    def runFile(self, fd, isRethrow = 0):
        """
        Read and execute commands from open file object.

        If script fails, then sends the exception info to debug (with the
        current verbosity level) and returns non-zero. Otherwise returns 0.        
        """
        line_no = 0
        while 1:
            line = fd.readline()
            line_no += 1
            if not line:
                return 0

            line = string.strip(line)
            if len(line) and string.lstrip(line)[0] == "#":
                continue # Ignore comments
            res = self.onecmd(line, 1, isRethrow)
            self.postcmd(1, line)
            if res:
                print >> sys.stderr, "Stop processing at line: ", line_no
                return res
        return 0

    def runString(self, string, isRethrow = 0):
        """
        Read and execute commands from string script.

        If script fails, then if isRethrow false send the exception info
        to debug (with the current verbosity level) and returns non-zero.
        On success return zero.

        If isRethrow is true then rethrow exceptions to external catcher.
        """
        return self.runFile(StringIO.StringIO(string), isRethrow)
