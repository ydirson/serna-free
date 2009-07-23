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
import os, platform

from SernaApi import *


class LinkNavigator(DocumentPlugin):

    def __init__(self, a1, a2): 
        DocumentPlugin.__init__(self, a1, a2)

        se = self.sernaDoc().structEditor()

        # Create and set double click watcher, 
        # Which will perform specific tasks on the double click event.
        
        self.__watcher = DblClickWatcher(self)
        se.setDoubleClickWatcher(self.__watcher)


class DblClickWatcher(SimpleWatcher):
    """Reacts on the double-click events
    """
    
    def __init__(self, plugin): 
        SimpleWatcher.__init__(self)

        # Note the weak reference.
        # Plugin keeps the reference to watcher, therefore
        # to avoid memory leak the watcher should keep weak
        # reference to plugin.
        self.__plugin = ref(plugin)

    def notifyChanged(self):
        """Called on double-click events.

           If the double-click event is intercepted executed, then
           return "False". Otherwise return "True", passing the event
           further.
        """
        
        # Get current position in the source XML tree (grove)
        pos = self.__plugin().sernaDoc().structEditor().getSrcPos()
        
        if pos.isNull():
            return True # No valid position, do nothing.
        
        # Check current position. If it is a text node, take its parent.
        first_node = XpathExpr("ancestor-or-self::ulink|ancestor-or-self::xref").\
                     eval(pos.node()).getNodeSet().firstNode()
        if not first_node:
            return True # The click is not within ulink or xref
        elif first_node.nodeName() == "ulink":
            self.navigateUlink(first_node)
        elif first_node.nodeName() == "xref":
            self.navigateXref(first_node)
        return False

    def navigateUlink(self, referencingNode):
        serna_doc = self.__plugin().sernaDoc()        
        link_attribute = referencingNode.asGroveElement().\
                         attrs().getAttribute("url")

        # No attribute - do nothing
        if not link_attribute:
            return
        link = link_attribute.value()

        # Look for *.html or *.htm file extension handler in
        # Serna configuration
        handlers = SernaConfig().root().getSafeProperty("file-handler-list").children()
        handlers.extend(SernaConfig().root().getSafeProperty("#file-handler-list").children())
        viewer = None
        for i in handlers:
            extension = i.getSafeProperty("extension").getString()
            if extension in ["html", "htm"]:
                viewer = i.getSafeProperty("executable").getString()
                break

        if not viewer or viewer.isEmpty():            
            serna_doc.showMessageBox(serna_doc.MB_CRITICAL,
                                     "Error",
                                     "HTML browser is not defined. " +
                                     "Check Tools->Preferences->Applications.",
                                     "OK")
            return
        
        res = os.system(str(viewer) + " " + str(link))
        if res:
            serna_doc.showMessageBox(serna_doc.MB_CRITICAL,
                                     "Error",
                                     "Error launching % s. Check Tools->Preferences"
                                     "->Applications." % str(viewer),
                                     "OK")
        return

    def navigateXref(self, referencingNode):
        link_attribute = referencingNode.asGroveElement().\
                         attrs().getAttribute("linkend")

        # No attribute - do nothing
        if not link_attribute:
            return
        link = link_attribute.value()

        # Find the referenced element by ID in the ID table
        referenced = referencingNode.grove().idManager().lookupElement(link)

        # No existing element - do nothing
        if not referenced:
            return
        
        # Scroll/set cursor to the target position
        se = self.__plugin().sernaDoc().structEditor()
        se.setCursorBySrcPos(GrovePos(referenced), se.getFoPos().node())

