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
from utils import Value, Creator
from codecs import getencoder
from PyQt4 import QtCore

import os

class Publisher(Value):
    def __init__(self, **kwargs):
        Value.__init__(self, **kwargs)

    def publish(self, caller, dsi, dstUri):
        self._callerNotified = False
        self._caller = caller

        self.srcUri = unicode(dsi.getProperty("doc-src").getString())
        self.dstUri = dstUri
        try:
            self._publish(dsi, dstUri)
        except PublishException, pe:
            raise PublishException(pe.getErrorString)

    def hasAdvancedOptions(self):
        return False

    def fillAdvancedOptions(self):
        return

    def cancel(self, kill=False):
        if self._cancel:
            self._cancel(kill)

    def transformComplete(self, exitCode, exitStatus):
        try:
            if self._transformComplete and \
                    self._transformComplete(exitCode, exitStatus):
                return
        except:
            pass

        if self._caller and not self._callerNotified:
            self._caller.publishComplete(exitCode, exitStatus)
            self._callerNotified = True

    def updateOutput(self, data):
        if self._caller and len(data):
            self._caller.updatePublisherOutput(data)

    def dump(self, indent=0):
        for k, v in self.attrs().iteritems():
            print "%s%s: '%s'" % (' ' * indent, k, v)

class PublisherCreator(Creator):
    def __init__(self, name, **kwargs):
        Creator.__init__(self, name, **kwargs)
        self._methods = {}

    def getTags(self, dsi):
        if hasattr(self, '_getTags'):
            return self._getTags(dsi)
        return []

    def make(self, **kwargs):
        return self._make(**kwargs)

    def _dump(self, indent, msg):
        print "%s%s" % (' '*indent, msg)

    def dump(self, indent):
        self._dump(indent, "PublisherCreator '%s':" % self.getName())
        indent += 2
        for k, v in self._methods.iteritems():
            self._dump(indent, "Publishing method '%s'" % k)
            v.dump(indent + 2)

