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
from SernaApi import *

class CheckEmptyPara(DocumentPlugin):
    """Traverses the document in order to find empty paras.

       If the empty paras are found, they are reported to the
       Serna message box.
    """
    def __init__(self, a1, a2): 
        DocumentPlugin.__init__(self, a1, a2)
        self.buildPluginExecutors(True)

    def aboutToSave(self):
        """Implementaiton of aboutToSave() event.
        """
        self.checkEmptyParas()

    def executeUiEvent(self, evName, cmd):
        """Execute the plugin's events.
        """
        # Call this function on any evName, because we have
        # only one event in this example anyway.
        self.checkEmptyParas()

    def checkEmptyParas(self):
        document = self.sernaDoc().structEditor().sourceGrove().document()
        node_set = XpathExpr("//para[not(node())]\
            [not(self::processing-instruction('se:choice'))]").\
                eval(document).getNodeSet()
        if node_set.size():
            self.sernaDoc().messageView().clearMessages()
            for n in node_set:
                self.sernaDoc().messageView().\
                    emitMessage("Empty <para> element found!", n)

