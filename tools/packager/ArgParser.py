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
import getopt, sys, string

class UndefinedHandler(Exception):
    def __init__(self, number):
        self.number = number
        Exception.__init__(self, number)
        
    def __str__(self):
        return "Undefined handler for %s arguments" % self.number

class Parser:
    """
    Parses a list of string arguments, including flags. Upon the number of
    found arguments calls do_N() (N is a integer equal to number of found
    arguments) or calls  if the function is not defined.
    """
    
    def __init__(self, list = [], flags = "", min = None, max = None):
        """
        Arguments:
        min - minimum number of arguments. If None then any number
              of arguments ok.
        max - maximum number of arguments. If None then exactly min number
              of arguments ok.

        If min is not None and max is -1 then not less than min arguments ok.
        """
        self.list = list
        self.flags = flags
        self.min = min
        self.max = max

    def run(self):
        """
        Parse flags and arguments and call function that handles corresponding
        numbers of arguments.

        If no numbered handlers given, then do_All() is called.
        """
        try:
            opts, args = getopt.getopt(self.list, self.flags)
        except getopt.GetoptError, x:
            return self.do_wrongOption(x.opt)
            
        length = len(args)
        if not self.checkLength(length):
            return self.do_wrongLength(length)

        method = getattr(self, "do_%s" % length, None)
        if method:
            return method(opts, args)
        return self.do_All(opts, args)

    def checkLength(self, length):
        """
        Check if the number of arguments is according to the specification.
        Return 1 if ok and 0 otherwise.
        """
        if self.max is None:
            if self.min is None:
                return 1
            if length != self.min:
                return 0
        elif self.max == -1:
            if length >= self.min:
                return 1
            else:
                return 0
        else:
            if not (length >= self.min and length <= self.max):
                return 0
        return 1

    def do_wrongOption(self, option):
        pass

    def do_wrongLength(self, length):
        pass

    def do_undefinedHandler(self, length):
        raise UndefinedHandler(length)

    def do_All(self, opts, args):
        return self.do_undefinedHandler(len(self.list))
