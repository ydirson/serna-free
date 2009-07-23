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
# This example allows to select a topic from another file and insert
# corresponding topicref.
#
from SernaApi import *
from weakref import *

# Watcher for the change of the current selected tree view item
class CurrentItemWatcher(PropertyNodeWatcher):
    def __init__(self, dp):
        PropertyNodeWatcher.__init__(self)
        self.dp_ = ref(dp)
    
    def propertyChanged(self, pn):
        dialog = self.dp_().dialog()
        listview = dialog.findItemByClass("ListView")
        href_line_edit = dialog.findItemByName("hrefLineEdit")
        list_item = listview.getByTreelocString(pn.getString())
        if list_item:
            id = list_item.itemProps().getProperty("id")
        else:
            id = PropertyNode()
        href = SString("")
        if id and href_line_edit:
            href = self.dp_().sysid() + SString("#") + id.getString()
            href_line_edit.itemProps().makeDescendant("text").setString(href)
        dialog.findItemByName("insertButton").action().\
            setEnabled(not href.isEmpty())

class DitaTopicRefPlugin(DocumentPlugin):
    def __init__(self, a1, a2):
        DocumentPlugin.__init__(self, a1, a2)
        self.buildPluginExecutors(1)
        self.initialized_ = 0

    def postInit(self):
        # create dialog; do not show it yet
        self.topicRefDialog_ = self.buildUiItem("ditaTopicRefDialog")
        self.topicListView_  = self.topicRefDialog_.findItemByClass("ListView")
        if not self.topicListView_:
            return
        self.current_ = self.topicListView_.itemProps().\
            makeDescendant("#current-item")
        # watch for the change of the current tree view item
        self.watcher_ = CurrentItemWatcher(self)
        self.current_.addWatcher(self.watcher_) 
        self.initialized_ = 1

    def preClose(self):
        # delete dialog before closing document
        if self.initialized_ and self.topicRefDialog_.parent():
            self.topicRefDialog_.remove()
        return 1

    def setGrove(self, grove):
        self.grove_ = grove
        if not self.initialized_:
            return
        # refresh tree view
        self.topicListView_.removeAllChildren()
        if self.grove_:
            make_topic_item(self.grove_.document().documentElement(), \
                self.sernaDoc(), self.topicListView_)
            self.current_.setString("0")
            self.watcher_.propertyChanged(self.current_)
        self.topicRefDialog_.update()
        
    # react to UI events
    def executeUiEvent(self, evName, uiAction):
        if evName == "ShowDtrDialog":
            if self.initialized_ and not self.topicRefDialog_.parent():
                self.sernaDoc().appendChild(self.topicRefDialog_)
            else:
                self.topicRefDialog_.setItemVisible(True)
            return
        if evName == "CloseDtrDialog":
            if self.initialized_:
                self.topicRefDialog_.remove()
            return
        if evName == "BrowseSourceDocument":
            pn = PropertyNode("")
            pn.makeDescendant("caption").setString("Select DITA Topic File")
            if not self.executeCommandEvent("ShowFileDialog", pn, pn):
                return
            file = pn.getProperty("url").getString()
            line_edit = self.topicRefDialog_.findItemByName("filenameLineEdit")
            if file.isEmpty() or not line_edit:
                return
            line_edit.itemProps().makeDescendant("text").setString(file)
            self.setGrove(Grove.buildGroveFromFile(file, Grove.pureXmlData,
                0, self.sernaDoc().structEditor().catalogManager()))
            self.sernaDoc().messageView().update()
            return
        
        if evName == "InsertDitaTopicRef":
            se = self.sernaDoc().structEditor()
            pos = se.getCheckedPos()
            if not self.initialized_ or not pos.node():
                return
            # Prepare attribute values for topicref element
            attr_set = PropertyNode("")
            self.add_attr_prop("href", "hrefLineEdit", attr_set)
            self.add_attr_prop("navtitle", "navtitleLineEdit", attr_set)
            self.add_multi_action_prop("scope", "scopeComboBox",
                                       "inscription", attr_set)
            self.add_multi_action_prop("type", "typeComboBox",
                                       "inscription", attr_set)

            # Insert topicref element with specified attributes
            se.executeAndUpdate(se.groveEditor().insertElement(
                pos, "topicref", attr_set));
            se.grabFocus() # set input focus back into the document 

    # utility function for setting attribute properties from named UI controls
    def add_attr_prop(self, attrName, itemName, attrSet):
        item = self.topicRefDialog_.findItemByName(itemName)
        if not item:
            return
        text = item.get("text")
        if not text.isEmpty():
            attrSet.appendChild(PropertyNode(attrName, text))

    def add_multi_action_prop(self, attrName, itemName, propName, attrSet):
        item = self.topicRefDialog_.findItemByName(itemName)
        if not item:
            return
        curr_action = item.currActionProp()
        if not curr_action:
            return
        text = curr_action.getSafeProperty(propName).getString()
        if not text.isEmpty():
            attrSet.appendChild(PropertyNode(attrName, text))

    # returns instance of topicref dialog
    def dialog(self):
        return self.topicRefDialog_
    
    # returns top sysid (path to the root entity of referenced document)
    def sysid(self):
        return self.grove_.topSysid()
    
# Make single item in tree view
def make_topic_item(node, doc, parentItem):
    elem = node.asGroveElement()
    if not elem:
        return
    attr = elem.attrs().getAttribute("id")
    if not attr:
        return;
    # create properties for tree view item
    props = PropertyNode("properties")
    props.makeDescendant("is-open").setBool(1)
    col = props.makeDescendant("data").makeDescendant("column")
    
    # set insciption on the tree view item: [id] + title
    col.makeDescendant("text").setString(
        SString("[") + attr.value() + SString("] ") + get_title(elem))
    # col.makeDescendant("icon").setString(icon)  # can add optional icon
    
    # save ID value in tree view item properties 
    props.makeDescendant("id").setString(attr.value())

    # make tree view item, attach it to the parent item
    selfItem = doc.makeItem("ListViewItem", props)
    parentItem.appendChild(selfItem)
    
    # recurse
    for c in node.children():
        make_topic_item(c, doc, selfItem)    

# Extract DITA topic title text from the <topic> element
def get_title(elem):
    titlestr = ""
    for n in elem.children():
        if n.nodeName() == "title":
            for c in n.children():
                if c.asGroveText():
                    titlestr += str(c.asGroveText().data())
            break
    return titlestr
