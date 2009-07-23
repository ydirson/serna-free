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
import sys
from SernaApi import *
from qt import *
from icons import *
from groveutil import *
from InsertReferenceDialog import InsertReferenceDialog

class LinkTargetData:
    def __init__(self, id, element, title, uri):
        self.id_ = id
        self.element_ = element
        self.title_ = title
        self.uri_ = uri
        
############################################################################

class InsertReferenceDialogImpl(InsertReferenceDialog):
    def __init__(self, plugin, parent, structEditor, name, func, isLocal = False ):
        InsertReferenceDialog.__init__(self, parent)
        layout_widget = self.child("layout3")
        layout_widget.setSizePolicy(QSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding,1,0,layout_widget.sizePolicy().hasHeightForWidth()))
        layout_widget = self.child("layout6")
        layout_widget.setSizePolicy(QSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding,2,0,layout_widget.sizePolicy().hasHeightForWidth()))

        self.listView_.setColumnWidthMode(0, QListView.Maximum)
        self.listView_.setColumnWidthMode(0, QListView.Manual)
        self.listView_.setColumnWidth(1, 50)

        self.filter_ = [ ["topic", "task", "concept", "reference"],
                         ["step", "substep"], ["li"], ["example"], \
                         ["section"], ["fig"], ["table"] ]
        self.srcGrove_ = structEditor.sourceGrove()
        self.structEditor_ = structEditor
        self.plugin_ = plugin
        self.name_ = name
        self.func_ = func
        self.isLocal_ = isLocal
        self.treeView_.setSorting(-1)
        self.listView_.setSorting(-1)
        self.treeView_.header().hide()
        self.srcFile_ = QFileInfo(self.srcGrove_.topSysid().__str__())
        self.curDir_ = QDir(self.srcFile_.dirPath(True))
        self.curDir_.setMatchAllDirs(True)
        if name == "xref":
            if isLocal:
                self.setCaption(qApp.translate("DITA",\
                    "Insert Local Reference"))
            else:
                self.setCaption(qApp.translate("DITA",\
                    "Insert External Reference"))
        elif name == "link":
            self.setCaption(qApp.translate("DITA",\
                "Insert Related Link"))
        elif name == "topicref":
            self.setCaption(qApp.translate("DITA",\
                "Insert Topic Reference"))
        elif name == "conref":
            if isLocal:
                self.setCaption(qApp.translate("DITA", "Insert Local Conref"))
            else:
                self.setCaption(qApp.translate("DITA",\
                    "Insert External Conref"))
        self.helpButton_.hide() #TODO enable when help is ready
        if isLocal:
            self.treeView_.hide()
            self.extComboBox_.hide()
            self.filterComboBox_.hide()
            self.addressLabel_.hide()
            self.addressEdit_.hide()
            self.browseButton_.hide()
            self.filePath_ = self.srcFile_.fileName()
            self.parseGrove(self.srcGrove_)
            self.adjustSize()
        else:
            self.extensionChanged()

    def fillTreeView(self, dir):
        if not dir.isReadable():
            return;
        self.treeView_.clear()
        self.treeView_.setUpdatesEnabled(False)
        files = dir.entryInfoList(QDir.DefaultFilter, QDir.DirsFirst or QDir.Name)
        if not(len(dir.path())==3 and str(dir.path().mid(1,1))==":") and \
           len(files) > 0:
            files.pop(0)   
        files.reverse()
        for it in files:
            item = QListViewItem(self.treeView_, it.fileName())
            if it.isDir():
                if it.fileName() == "..":
                    item.setPixmap(0, QPixmap(pix_folder_open))
                else:
                    item.setPixmap(0, QPixmap(pix_folder))
            else:
                if it.extension() == "xml":
                    item.setPixmap(0, QPixmap(pix_file_xml))
                else:
                    item.setPixmap(0, QPixmap(pix_file))
        self.treeView_.clearSelection()
        self.treeView_.setUpdatesEnabled(True)

    def fileOrFolderChanged(self):
        item = self.treeView_.selectedItem()
        if not item:
            return
        if self.curDir_.cd(item.text(0), False):
            self.fillTreeView(self.curDir_)
        else:
            self.parseGrove()

    def extensionChanged(self):
        self.curDir_.setNameFilter(self.extComboBox_.currentText())
        self.fillTreeView(self.curDir_)

    def prepareFilePath(self, path):
        parentItem = self.listView_
        self.listView_.clear()
        files = self.curDir_.entryInfoList(QDir.Files, QDir.Name)
        it = None
        is_found = False
        for it in files:
            if it.fileName() == path:
                is_found = True
                break
        if is_found and self.srcFile_.absFilePath() != it.absFilePath():
            self.filePath_ = it.absFilePath()
            src_path = self.srcFile_.dir().absPath()
            cut_start = src_path.length() + 1
            if cut_start == 4 and src_path.mid(1) == QString(":/"):
                cut_start = cut_start  - 1 # for windows root paths
            if self.filePath_.contains(src_path):
                self.filePath_ = self.filePath_.mid(cut_start)

        fi = QFileInfo(self.filePath_)
        full_path = fi.absFilePath()
        if fi.isRelative():
            full_path = QFileInfo(self.curDir_,
                                  fi.fileName()).absFilePath()

        if self.filterComboBox_.currentItem() == 0:
            href_value = self.relativePath(
                SString(full_path.__str__())).__str__()
            parentItem = QListViewItem(self.listView_, href_value,
                                       '', href_value)
            parentItem.setOpen(True)
        self.addressEdit_.setText(self.filePath_.__str__())
        return parentItem

    def parseGrove(self, grove = None, url = None):
        parentItem = self.listView_
        is_local = True
        self.grove_ = grove
        if url: #webdav
            is_local = False
            self.curDir_.setCurrent(QFileInfo(url).dirPath(True))
            self.filePath_ = url
            parentItem = self.prepareFilePath(QFileInfo(url).fileName())
            self.grove_ = Grove.buildGroveFromFile(
                url, Grove.noComments, True,
                self.structEditor_.catalogManager())
        elif not grove:
            is_local = False
            item = self.treeView_.selectedItem()
            if not item:
                return         
            self.filePath_ = self.srcFile_.fileName()
            parentItem = self.prepareFilePath(item.text(0))
            fi = QFileInfo(self.filePath_)
            full_path = fi.absFilePath()
            if fi.isRelative():
                full_path = QFileInfo(self.curDir_,
                                      fi.fileName()).absFilePath()
            self.grove_ = Grove.buildGroveFromFile(
                full_path.__str__(), Grove.noComments, True,
                self.structEditor_.catalogManager())
        filters = []
        if self.filterComboBox_.currentItem() > 0:
            filters = self.filter_[self.filterComboBox_.currentItem() - 1]
        self.fillRefsView(self.grove_.document().documentElement(),
                          parentItem, filters, is_local)

    def fillRefsView(self, node, parentItem, filters, isLocal):
        elem = node.asGroveElement()
        if not elem:
            return
        id_attr = elem.attrs().getAttribute("id")
        success = (len(filters) == 0)
        for f in filters:
            if f == elem.nodeName():
                success = True
        if id_attr and success:
            content = XpathExpr(".").eval(elem).getString()
            if not content:
                content = SString()
            content = content.simplifyWhiteSpace().left(20)
            if content.length() > 18:
                content = content + "..."
            field = content + "  [" + id_attr.value() + "]"
            href_value = self.composeIdString(elem, isLocal).__str__()
            selfItem = QListViewItem(parentItem, field.__str__(), 
                       elem.nodeName().__str__(), href_value)
            selfItem.setOpen(True)
        else:
            selfItem = parentItem
        for child in elem.children():
            self.fillRefsView(child, selfItem, filters, isLocal)    
        if self.listView_.firstChild():
            self.listView_.setSelected(self.listView_.firstChild(), True)

    def relativePath(self, path):
        url = Url(self.srcGrove_.topSysid())
        url = Url(url.get(Url.DIRPATH))
        return url.relativePath(Url(path)).asString()
        
    def composeIdString(self, elem, isLocal):
        topic = elem
        while topic:
            class_attr = topic.attrs().getAttribute("class")
            if class_attr and class_attr.value().find(" topic/topic ") >= 0:
                break
            topic = topic.parent().asGroveElement()

        if not topic:
            return SString()
        
        topic_id = topic.attrs().getAttribute("id").value()
        href_url = SString('')

        if not isLocal:
            href_url = Url(self.srcGrove_.topSysid())
            href_url = href_url.relativePath(
                Url(self.grove_.topSysid())).asString()

        if elem != topic:
            elem_id = elem.attrs().getAttribute("id").value()
            return href_url + '#' + topic_id + '/' + elem_id

        return href_url + '#' + topic_id
        
            
    def filterChanged(self):
        self.parseGrove()

    def insertLink(self):
        sel_item = self.listView_.selectedItem()
        if not sel_item:
            return
        elem_name = sel_item.text(1).__str__()
        id_str = sel_item.text(0).__str__()
        id_str = id_str[id_str.find('[')+1:-1]
        result_col = ""
        if self.name_ == "conref":
            item = self.listView_.firstChild()
            if item.text(0) == self.filePath_:
                item = item.firstChild()
            if item and item != sel_item:
                id_txt = item.text(0).__str__()
                result_col = id_txt[id_txt.find('[')+1:-1] + "/" + result_col
            #item = sel_item.parent()
            #while item and item.text(0) != self.filePath_:
            #    id_txt = item.text(0).__str__()
            #    result_col = id_txt[id_txt.find('[')+1:-1] + "/" + result_col
            #    item = item.parent()
        if sel_item.text(0) != self.filePath_:
            result_col = "#" + result_col + id_str
        if not self.isLocal_ and  self.srcFile_.fileName() != self.filePath_:
            result_col = self.filePath_.__str__() + result_col
        if self.name_ == "link" and not get_node("//*[@id='"+ id_str + "']/title",\
           self.grove_.document().documentElement()):
            elem_name = ""
        #self.func_(self.name_, result_col, elem_name)
        self.func_(self.name_, SString(str(sel_item.text(2))), elem_name)

    def insertLinkAndClose(self):
        self.insertLink()
        self.hide()
        self.deleteLater()
        
    def selectionChanged(self):
        self.insertButton_.setEnabled(self.listView_.selectedItem() != None)
        self.insertAndCloseButton_.setEnabled(self.listView_.selectedItem() != None)

    def browse(self):
        pn = PropertyNode("")
        if not self.plugin_.executeCommandEvent("OpenUrlDialog",pn, pn):
            return;
        url = pn.getSafeProperty("doc-src").getString()
        if url.isEmpty():
            return
        self.parseGrove(None, url.__str__())
