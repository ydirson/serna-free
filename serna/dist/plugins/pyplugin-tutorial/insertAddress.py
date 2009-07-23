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
from PyQt4.QtGui import QDialog

Ui_InsertAddressDialog, DialogBase = \
    load_qt_uitype(__file__, "InsertAddressDialog.ui")

class InsertAddressDialog(Ui_InsertAddressDialog, DialogBase):
    def __init__(self, *args):
        DialogBase.__init__(self, *args)
        self.setupUi(self)

class InsertAddress(DocumentPlugin):
    """Inserts address to a letter with a dialog.
    """
    
    def __init__(self, a1, a2):
        DocumentPlugin.__init__(self, a1, a2)
        self.buildPluginExecutors(True)

    def postInit(self):
        self.se  = self.sernaDoc().structEditor()
        self.doc = self.se.sourceGrove().document()

    def executeUiEvent(self, evName, uiAction):
        if "InsertAddress" == evName:
            self.showDialog()
            # Set input focus back to the document edit window
            self.se.grabFocus()

    def showDialog(self):
        """Execute event that shows up a dialog"""

        # If the <address> element already exists, fill dialog with
        # its values.
        node_set = XpathExpr("//address\
            [not(self::processing-instruction('se:choice'))]").\
                eval(self.doc).getNodeSet()
        
        # construct instance of the Qt dialog
        dialog = InsertAddressDialog(self.sernaDoc().widget())
        
        if node_set.firstNode():
            for i in node_set.firstNode().children():
                # Put the string value from node <xxx> to xxxLineEdit
                #
                # We get the text value of the node by evaluating its
                # XPath string value. Note, that the next expression
                # is evaluated with the current node as context node.
                line_edit = getattr(dialog, unicode(i.nodeName()) + "LineEdit")
                if line_edit:
                    text = XpathExpr("string()").eval(i).getString()
                    line_edit.setText(unicode(text))
        
        if QDialog.Accepted == dialog.exec_():
            self.acceptAddress(dialog)

    def acceptAddress(self, dialog):
        """Execute event that inserts the values from dialog
           when user presses OK button.
        """

        # Firstly, remove the old <address> if exists
        node_set = XpathExpr("//address\
        [not(self::processing-instruction('se:choice'))]").eval(self.doc).getNodeSet()
        if node_set.firstNode():
            self.se.executeAndUpdate(
                self.se.groveEditor().removeNode(node_set.firstNode()));

        # Build element tree, taking text from the dialog, 
        # and insert to the document.
        address = ["street", "city", "state", "zip", "country"]
        fragment = GroveDocumentFragment()
        address_element = GroveElement("address")
        fragment.appendChild(address_element)

        for i in address:
            text = unicode(getattr(dialog, i + "LineEdit").text())
            address_child = GroveElement(i)
            if len(text):
                address_child.appendChild(GroveText(text))
            address_element.appendChild(address_child)

        # Find the position for the new <address> node.
        # This is either right after <title> and <date> elements if
        # they exist, or this is the first child of the document.
        node_set = XpathExpr("/*/title|/*/date").eval(self.doc).getNodeSet()
        if node_set.size():
            position_node = node_set.list()[-1]
        else:
            position_node = None
        if position_node:
            grove_pos = GrovePos(position_node.parent(),
                                 position_node.nextSibling())
        else:
            grove_pos = GrovePos(self.doc.documentElement(),
                                 self.doc.documentElement().firstChild())
            
        self.se.executeAndUpdate(self.se.groveEditor().paste(fragment, grove_pos)) 



