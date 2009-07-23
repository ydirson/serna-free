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
# 
# Commonly-used utilities for working with document tree
#
from SernaApi import *
import qtxml, sys

# Creates element with optional attributes and children (recursively)
def build_element(element_name, children, attributes = None):
    element = GroveElement(element_name)
    # Set element attributes
    if attributes:
        for attribute in attributes:
            attr = GroveAttr(attribute[0], attribute[1])
            element.attrs().appendChild(attr)
            
    if isinstance(children, str):
        element.appendChild(GroveText(children))
        
    if isinstance(children, list):
        for child in children:
            child_element = build_element(child[0], child[1])
            element.appendChild(child_element)

    if isinstance(children, GroveElement):
        clone = children.cloneNode(True, element)
        element.appendChild(clone)
        
    return element

def build_tree_element(content):
    doc = qtxml.QDomDocument()
    doc.setContent(content)
    return build_sub_element(doc.firstChild())

def build_sub_element(node, parent = None):
    element = None
    while not node.isNull():
        if node.isElement():
            element = GroveElement(node.nodeName().__str__())
            if parent:
                parent.appendChild(element)
            attrs = node.toElement().attributes()
            for i in range(attrs.count()):
                attr = attrs.item(i).toAttr()
                element.attrs().appendChild(GroveAttr(attr.name().__str__(),\
                                                      attr.value().__str__()))
            build_sub_element(node.firstChild(), element)
        if node.isText():
            parent.appendChild(GroveText(node.toText().data().__str__()))
        node = node.nextSibling();
    return element

# Returns element node attribute value
def get_attribute(grove_node, attribute_name):
    attributes = grove_node.asGroveElement().attrs()
    return str(attributes.getAttribute(attribute_name).value())
    
# Returns list of text data, collected from nodes returned
# by evaluated Xpath expression
def get_data_from_expr(expr, context):
    data = []
    for node in get_nodes(expr, context):
        data.append(get_node_text(node))
    return data
    
# Returns text from the first text node from nodes returned
# by evaluated Xpath expression
def get_datum_from_expr(expr, context):
    node = get_node(expr, context)
    if not node:
        return None
    return get_node_text(node)


# Returns text from the given node
def get_node_text(grove_node):
    result = str()
    child = grove_node.getChild(0)
    if not child:
        return result
    return child.asGroveText().data().__str__()

def get_datum_text_tree(grove_node):
    result = SString()
    for child in grove_node.children():
        if child.asGroveText():
            result = result + child.asGroveText().data()
        elif child.asGroveElement():
            name = str(child.nodeName())
            result = result + SString('<') + name
            attr = child.asGroveElement().attrs().firstChild()
            attrstr = ""
            while attr:
                attrstr = " " + attr.nodeName().__str__() + \
                          "='" +  attr.value().__str__() + "'"
                attr = attr.nextSibling()
            result = result + SString(attrstr) + SString('>')
            result = result + get_datum_text_tree(child)
            result = result + SString("</") + name + SString('>') 
    return result

# Returns first node (from the node-set) of evaluated XPATH expression
def get_node(expr, context):
    if not context:
        return None    
    nodes = get_nodes(expr, context)
    if len(nodes) > 0:
        return nodes[0]
    return None
    
def get_nodes(expr, context):
    xpath_value = XpathExpr(expr).eval(context)
    return xpath_value.getNodeSet().list()
    
def get_node_set(expr, context):
    xpath_value = XpathExpr(expr).eval(context)
    return xpath_value.getNodeSet()
    
def print_tree(node):
    if not node.asGroveElement():
        return;
    print("Node:" + str(node.nodeName()))
    for cur in node.children():
        print_tree(cur)

def print_prop_tree(node):
    if not node:
        return;
    print("Node:" + str(node.name()) + " Value:" + str(node.getString()) )
    for cur in node.children():
        print_prop_tree(cur)

# Custom XSLT function which returns fixed value
class StringFunction(XsltExternalFunction):
    def __init__(self, name, nsUri, fixed_value):
        XsltExternalFunction.__init__(self, name, nsUri)
        self.__value = fixed_value
        
    def eval(self, valueList):
        return XpathValue(self.__value)

# find ancestor element belong to the any of the mentioned DITA classes
def find_dita_class(node, *csn):
    elem = node.asGroveElement()
    while elem:
        dita_class = unicode(elem.attrs().getAttribute("class").value())
        for str in csn:
            if dita_class.find(str + ' ') > 0:
                return elem
        elem = elem.parent().asGroveElement()
    return GroveNode()

def traverse_to_element(plugin):
    pos = plugin.sernaDoc().structEditor().getSrcPos()  
    if pos.isNull():
        return GroveNode()
    node = pos.node()
    if GrovePos.TEXT_POS == pos.type():
        node = node.parent() 
    return node
