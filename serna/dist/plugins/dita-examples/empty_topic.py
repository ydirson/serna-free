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
# This plugin traverses the document tree on the DocumentSave event,
# and reports to the message box empty DITA topic elements which do not 
# have topicref attribute.
#
from SernaApi import *

class CheckEmptyDitaTopics(DocumentPlugin):
    def __init__(self, a1, a2): 
        DocumentPlugin.__init__(self, a1, a2)
        self.buildPluginExecutors(1)

    # Implementaiton of aboutToSave() event. Traverse the tree here.
    def aboutToSave(self):
        se   = self.sernaDoc().structEditor()
        root = se.sourceGrove().document().documentElement()
        self.check_topics(root)

    def check_topics(self, n):
        # check whether current node name is "p" and it is empty
        if n.nodeName() == "p" and not n.firstChild():
            self.sernaDoc().messageView().\
                emitMessage("DITA: Empty <p> element found!", n)
        # recursively check children
        for child in n.children():
            self.check_topics(child)

    def executeUiEvent(self, evName, cmd):
        self.aboutToSave()  # call the same function
