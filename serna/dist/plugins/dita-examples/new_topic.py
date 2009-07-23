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
# This plugin asks for DITA Topic title and ID when creating a new DITA
# document.
#

# Import Serna API and Python standard regular expression module (re)
from SernaApi import *
import re               

# This is a class which validates ID input field. Instance of this class
# watches the "text" property of the LineEdit UI item, and enables or 
# disables OK button.
class IdLineEditWatcher(PropertyNodeWatcher):
    def __init__(self, okButton):
        PropertyNodeWatcher.__init__(self)
        self.okButton_ = okButton
        self.idRegexp_ = re.compile("^\\w+$") 
        
    def propertyChanged(self, ptn):
        isOk = self.idRegexp_.match(str(ptn.getString()))
        self.okButton_.action().setEnabled(isOk != None)

# This class reacts to the changes in Title line edit, generates
# prototype id string and sets it to ID line edit.
class TitleLineEditWatcher(PropertyNodeWatcher):
    def __init__(self, idText):
        PropertyNodeWatcher.__init__(self)
        self.idText_ = idText
        
    def propertyChanged(self, ptn):
        new_id = str(ptn.getString().lower()).replace(" ", "")
        self.idText_.setString(new_id)

# This is a plugin instance class. 
class NewDitaTopic(DocumentPlugin):
    def __init__(self, a1, a2): 
        DocumentPlugin.__init__(self, a1, a2)
        self.buildPluginExecutors(1)        # build UiActions 

    # this method is called when new document is being created.
    def newDocumentGrove(self):
        # build dialog from it's description in plugin .spd file,
        # and find topic ID line edit tool in built UI tree
        self.dialog_  = self.buildUiItem("newDitaTopicDialog")
        self.topicId_ = self.dialog_.findItemByName("idLineEdit")
       
        # create watcher for "text" property of ID line edit tool
        idText = self.topicId_.property("text")
        self.idLineWatcher_ = IdLineEditWatcher(
            self.dialog_.findItemByName("okButton"))
        idText.addWatcher(self.idLineWatcher_)
    
        # create watcher for "text" property of Title line edit
        titleLineEdit = self.dialog_.findItemByName("titleLineEdit")
        titleText = titleLineEdit.property("text")
        self.titleWatcher_ = TitleLineEditWatcher(idText)
        titleText.addWatcher(self.titleWatcher_)
        self.idLineWatcher_.propertyChanged(idText)
        
        # make dialog appear. Note that we call setItemVisible(1) here; it is
        # only necessary in implementations of newDocumentGrove, because
        # main document view is not yet built.
        self.sernaDoc().appendChild(self.dialog_)
        self.dialog_.setItemVisible(True)

    # This method reacts to UI events. Names of UI events are defined in
    # <commandEvent> fields of UiAction definitions in .spd file.
    def executeUiEvent(self, evName, uiAction):
        if evName != "acceptNewTopicDialog":
            self.dialog_.remove()
            return
        # ok - take information from edit boxes, build subtree which consists
        # of topic/title/<text>, set ID on topic, and attach it to document
        topicTitle = self.dialog_.findItemByName("titleLineEdit")
        docroot = self.sernaDoc().structEditor().sourceGrove().document()
        docelem  = GroveElement("topic")
        doctitle = GroveElement("title")
        doctitle.appendChild(GroveText(topicTitle.get("text")))
        docelem.attrs().appendChild(GroveAttr("id", self.topicId_.get("text")))
        docelem.appendChild(doctitle)
        docroot.appendChild(docelem)
        self.dialog_.remove()

