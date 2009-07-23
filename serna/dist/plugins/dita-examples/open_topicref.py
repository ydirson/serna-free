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
# DITA topicref element.
#
from SernaApi import *
import os

class TopicClickWatcher(SimpleWatcher):
    def __init__(self, dp):
        SimpleWatcher.__init__(self)
        self.dp_ = dp

    def notifyChanged(self):
        se  = self.dp_().sernaDoc().structEditor()
        pos = se.getCheckedPos()
        if not pos or pos.node().nodeName() != "topicref":
            return 1
        hrefatt = pos.node().asGroveElement().attrs().getAttribute("href")
        if not hrefatt or hrefatt.value().isEmpty():
            return 1    
        ptn    = PropertyNode("doc-src-info")
        # use source document path as a base path instead of current directory
        path = os.path.dirname(str(se.sourceGrove().topSysid()))
        path = os.path.join(path, str(hrefatt.value()))
        # strip ID
        if path.find('#') > 0:
            path = path.split('#')[0]    
        ptn.makeDescendant("doc-src").setString(path)
        self.dp_().executeCommandEvent("OpenDocumentWithDsi", ptn)
        return 0

class OpenOnTopicRef(DocumentPlugin):
    def __init__(self, a1, a2): 
        DocumentPlugin.__init__(self, a1, a2)
    
    def postInit(self):
        self.clickWatcher_ = TopicClickWatcher(ref(self))
        self.sernaDoc().structEditor().setDoubleClickWatcher(self.clickWatcher_)
