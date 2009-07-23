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
from utils import dump_ptree

class PropertyNode(object):
    def __init__(self, name=None, value=None):
        self._name = name
        self._value = value
        self._firstChild = self._lastChild = self._nextSibling = None
        self._parent = None

    def __len__(self):
        return not self._name is None or not self._firstChild is None

    def name(self):
        return self._name

    def firstChild(self):
        if self._firstChild:
            return self._firstChild
        return PropertyNode()

    def nextSibling(self):
        if self._nextSibling:
            return self._nextSibling
        return PropertyNode()

    def parent(self):
        if self._parent:
            return self._parent
        return PropertyNode()

    def appendChild(self, node):
        if self._lastChild:
            self._lastChild._nextSibling = node
            self._lastChild = node
            node._nextSibling = None
        else:
            self._firstChild = self._lastChild = node
        node._parent = self

    def _findChild(self, name):
        child = self._firstChild
        while child:
            if child.name() == name:
                return child
            child = child.nextSibling()
        return PropertyNode()

    def getProperty(self, propName, create=False):
        names = propName.split('/')
        curNode = self
        for name in names:
            child = curNode._findChild(name)
            if not child:
                if create:
                    child = PropertyNode(name)
                    curNode.appendChild(child)
                else:
                    return PropertyNode()
            curNode = child
        return curNode

    def makeDescendant(self, name, value):
        prop = self.getProperty(name, True)
        prop.setString(value)
        return prop

    def getString(self):
        if self._value:
            return str(self._value)
        return ''

    def setString(self, value):
        self._value = value

    def setName(self, name):
        self._name = name

    def dump(self, indent=0):
        dump_ptree(self, indent)

def get_node_value(line):
    name, value = line.split(':', 1)
    return name.strip(), value.strip().strip("'")

def make_property_node(line):
    name, value = get_node_value(line)
    return PropertyNode(name, value)

def fill_property_node(node, line, lines, ind=0):
    if line:
        node.appendChild(make_property_node(line))
    for line in lines:
        if ind and line[:ind].strip():
            return line
        line = line.rstrip()
        if line[ind] == ' ':
            if node._lastChild:
                line = fill_property_node(node._lastChild, line, lines, ind + 2)
                if not line:
                    return None
                elif line[:ind].strip():
                    return line
            else:
                continue

        curNode = make_property_node(line)
        node.appendChild(curNode)

    return None

def read_property_tree(fname):
    node = None
    fobj = open(fname, 'r')
    lines = iter(fobj)
    for line in lines:
        if line.strip() and line[0] != ' ':
            node = make_property_node(line)
            fill_property_node(node, '', lines, 2)
            break

    return node
