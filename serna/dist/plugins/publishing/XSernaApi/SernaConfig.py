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
import os, re
from utils import dump_ptree
from PropertyNode import PropertyNode

__all__ = ['SernaConfig']

class Resolver(object):
    def __init__(self, props):
        self._props = props

    def __call__(self, mobj):
        vg = mobj.group("varref")
        if vg:
            if not self._props is None and self._props.firstChild():
                prop = self._props.getProperty(vg[1:])
                if prop:
                    return unicode(prop.getString())
            return unicode(os.environ.get(vg[1:], ''))

class SernaConfig(object):
    _root = PropertyNode()

    @staticmethod
    def resolveResource(propName, resPath, baseUri, localVars=None):
        resolver = Resolver(localVars)
        nsubs = 1
        while nsubs:
            resPath, nsubs = re.subn(r'(?P<varref>\$SERNA_[A-Z0-9_]+)',
                                     resolver, resPath)
        if not os.path.isabs(resPath):
            baseDir = os.path.dirname(baseUri)
            return baseDir + '/' + resPath

        return unicode(resPath)

    @staticmethod
    def init(**kwargs):
        for name, value in kwargs.iteritems():
            SernaConfig._root.makeDescendant(name, unicode(value))

    @staticmethod
    def getProperty(name):
        return SernaConfig._root.getProperty(name).getString()
