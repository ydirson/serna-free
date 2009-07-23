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
# This plugin example opens the document if user double-clicks within the
# Docbook link or ulink element.
#
import os
import platform     
import qt
from SernaApi import *
from groveutil import *
from weakref import *

# DblClickWatcher catches mouse double click events.
class DblClickWatcher(SimpleWatcher):   
    def __init__(self, plugin): 
        SimpleWatcher.__init__(self)
        self.__plugin = ref(plugin)
        se = self.__plugin().sernaDoc().structEditor()
        self.__sysid = unicode(se.sourceGrove().topSysid())

    def goLocal(self, node, id):
        if not id:
            return 1
        elem = get_node("//*[@id='" + id + "'][1]", node)
        if not elem:
            return 1
        se = self.__plugin().sernaDoc().structEditor()
        se.setCursorBySrcPos(GrovePos(elem, elem.firstChild()),
            se.getFoPos().node())
        return 0

    def openDocument(self, path):
        item = self.__plugin().sernaDoc().parent().firstChild()
        while item.asSernaDoc():
            doc_src = item.asSernaDoc().getDsi().getProperty("doc-src")
            if doc_src:
                if path == doc_src.getString():
                    item.asSernaDoc().setActive()
                    return
            item = item.nextSibling()

        ptn = PropertyNode("doc-src-info")
        ptn.makeDescendant("doc-src").setString(path)
        self.__plugin().executeCommandEvent("OpenDocumentWithDsi", ptn)
    
    def notifyChanged(self):
        elem = find_dita_class(traverse_to_element(self.__plugin()), \
            "topic/link", "topic/xref", "map/topicref")
        if not elem:
            return 1
        attr = elem.attrs().getAttribute("href")
        if attr.isNull() or attr.value().isEmpty():
            return 1
        path  = unicode(attr.value())
        pos   = path.find('#')
        idref = ""
        if pos >= 0:
            idref = path[pos+1:]
            path  = path[0:pos]
        if not path:
            return self.goLocal(elem, idref)
        sysurl = qt.QUrl(self.__sysid)
        if qt.QUrl.isRelativeUrl(path):
            tpath = qt.QUrl(sysurl)
            tpath.setFileName("")
            tpath.addPath(path)
            path = unicode(tpath.toString())
        if sysurl == qt.QUrl(path):
            return self.goLocal(elem, idref)
        format = elem.attrs().getAttribute("format")
        if format.isNull() or format.value().isEmpty():
            format = u""
        else:
            format = unicode(format.value())
        if format in (u"xml", u"dita", u"ditamap") or path.endswith(u".xml") or \
           path.endswith(u".dita") or path.endswith(u".ditamap"):
            self.openDocument(path)
            return 0
        if format is None or 0 == len(format):
            path = unicode(qt.QUrl(path).path())
        # Get path extension
            pos = path.rfind('.')
            if pos >= 0:
                format = path[pos+1:]
        ptn = PropertyNode("browser-params")
        ptn.makeDescendant("url").setString(norm_slashes(path))
        ptn.makeDescendant("extension").setString(unicode(format))
        rv = self.__plugin().executeCommandEvent("LaunchBrowser", ptn)
        return 0

def norm_slashes(s):
    return s.replace('\\', '/')
