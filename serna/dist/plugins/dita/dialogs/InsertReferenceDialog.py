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
# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'InsertReferenceDialog.ui'
#
# Created: Пн 13. мар 10:16:30 2006
#      by: The PyQt User Interface Compiler (pyuic) 3.15
#
# WARNING! All changes made in this file will be lost!


import sys
from qt import *


class InsertReferenceDialog(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        if not name:
            self.setName("InsertReferenceDialog")


        InsertReferenceDialogLayout = QGridLayout(self,1,1,11,6,"InsertReferenceDialogLayout")

        self.helpButton_ = QPushButton(self,"helpButton_")
        self.helpButton_.setAutoDefault(0)

        InsertReferenceDialogLayout.addWidget(self.helpButton_,2,0)

        layout7 = QHBoxLayout(None,0,6,"layout7")

        self.addressLabel_ = QLabel(self,"addressLabel_")
        layout7.addWidget(self.addressLabel_)

        self.addressEdit_ = QLineEdit(self,"addressEdit_")
        self.addressEdit_.setSizePolicy(QSizePolicy(QSizePolicy.Expanding,QSizePolicy.Fixed,0,0,self.addressEdit_.sizePolicy().hasHeightForWidth()))
        layout7.addWidget(self.addressEdit_)

        self.browseButton_ = QPushButton(self,"browseButton_")
        self.browseButton_.setSizePolicy(QSizePolicy(QSizePolicy.Minimum,QSizePolicy.Fixed,0,0,self.browseButton_.sizePolicy().hasHeightForWidth()))
        layout7.addWidget(self.browseButton_)

        InsertReferenceDialogLayout.addMultiCellLayout(layout7,0,0,0,4)

        self.insertButton_ = QPushButton(self,"insertButton_")
        self.insertButton_.setEnabled(0)
        self.insertButton_.setAutoDefault(0)

        InsertReferenceDialogLayout.addWidget(self.insertButton_,2,2)

        self.insertAndCloseButton_ = QPushButton(self,"insertAndCloseButton_")
        self.insertAndCloseButton_.setEnabled(0)
        self.insertAndCloseButton_.setSizePolicy(QSizePolicy(QSizePolicy.Fixed,QSizePolicy.Fixed,0,0,self.insertAndCloseButton_.sizePolicy().hasHeightForWidth()))

        InsertReferenceDialogLayout.addWidget(self.insertAndCloseButton_,2,3)

        self.cancelButton_ = QPushButton(self,"cancelButton_")
        self.cancelButton_.setAutoDefault(0)

        InsertReferenceDialogLayout.addWidget(self.cancelButton_,2,4)
        spacer5 = QSpacerItem(220,20,QSizePolicy.Expanding,QSizePolicy.Minimum)
        InsertReferenceDialogLayout.addItem(spacer5,2,1)

        self.splitter2 = QSplitter(self,"splitter2")
        self.splitter2.setOrientation(QSplitter.Horizontal)
        self.splitter2.setHandleWidth(1)

        LayoutWidget = QWidget(self.splitter2,"layout3")
        layout3 = QVBoxLayout(LayoutWidget,0,2,"layout3")

        self.treeView_ = QListView(LayoutWidget,"treeView_")
        self.treeView_.addColumn(self.__tr("Files"))
        self.treeView_.setSizePolicy(QSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding,1,0,self.treeView_.sizePolicy().hasHeightForWidth()))
        self.treeView_.setResizeMode(QListView.AllColumns)
        layout3.addWidget(self.treeView_)

        self.extComboBox_ = QComboBox(0,LayoutWidget,"extComboBox_")
        self.extComboBox_.setSizePolicy(QSizePolicy(QSizePolicy.Minimum,QSizePolicy.Fixed,1,0,self.extComboBox_.sizePolicy().hasHeightForWidth()))
        layout3.addWidget(self.extComboBox_)

        LayoutWidget_2 = QWidget(self.splitter2,"layout6")
        layout6 = QVBoxLayout(LayoutWidget_2,0,2,"layout6")

        self.listView_ = QListView(LayoutWidget_2,"listView_")
        self.listView_.addColumn(self.__tr("Select Target"))
        self.listView_.addColumn(self.__tr("Type"))
        self.listView_.header().setClickEnabled(0,self.listView_.header().count() - 1)
        self.listView_.setSizePolicy(QSizePolicy(QSizePolicy.Expanding,QSizePolicy.Expanding,2,0,self.listView_.sizePolicy().hasHeightForWidth()))
        self.listView_.setResizePolicy(QListView.AutoOneFit)
        self.listView_.setAllColumnsShowFocus(1)
        self.listView_.setRootIsDecorated(1)
        self.listView_.setResizeMode(QListView.AllColumns)
        layout6.addWidget(self.listView_)

        self.filterComboBox_ = QComboBox(0,LayoutWidget_2,"filterComboBox_")
        self.filterComboBox_.setSizePolicy(QSizePolicy(QSizePolicy.Minimum,QSizePolicy.Fixed,2,0,self.filterComboBox_.sizePolicy().hasHeightForWidth()))
        layout6.addWidget(self.filterComboBox_)

        InsertReferenceDialogLayout.addMultiCellWidget(self.splitter2,1,1,0,4)

        self.languageChange()

        self.resize(QSize(600,435).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.connect(self.cancelButton_,SIGNAL("clicked()"),self.reject)
        self.connect(self.insertButton_,SIGNAL("clicked()"),self.insertLink)
        self.connect(self.extComboBox_,SIGNAL("activated(int)"),self.extensionChanged)
        self.connect(self.filterComboBox_,SIGNAL("activated(int)"),self.filterChanged)
        self.connect(self.insertAndCloseButton_,SIGNAL("clicked()"),self.insertLinkAndClose)
        self.connect(self.treeView_,SIGNAL("returnPressed(QListViewItem*)"),self.fileOrFolderChanged)
        self.connect(self.treeView_,SIGNAL("clicked(QListViewItem*)"),self.fileOrFolderChanged)
        self.connect(self.listView_,SIGNAL("doubleClicked(QListViewItem*)"),self.insertLink)
        self.connect(self.listView_,SIGNAL("selectionChanged()"),self.selectionChanged)
        self.connect(self.browseButton_,SIGNAL("clicked()"),self.browse)

        self.setTabOrder(self.treeView_,self.listView_)
        self.setTabOrder(self.listView_,self.helpButton_)
        self.setTabOrder(self.helpButton_,self.insertButton_)
        self.setTabOrder(self.insertButton_,self.cancelButton_)


    def languageChange(self):
        self.setCaption(self.__tr("Insert Reference"))
        self.helpButton_.setText(self.__tr("&Help"))
        self.helpButton_.setAccel(self.__tr("Alt+H"))
        self.addressLabel_.setText(self.__tr("URL:"))
        self.browseButton_.setText(self.__tr("&Browse DAV..."))
        self.browseButton_.setAccel(self.__tr("Alt+B"))
        self.insertButton_.setText(self.__tr("&Insert"))
        self.insertButton_.setAccel(self.__tr("Alt+I"))
        self.insertAndCloseButton_.setText(self.__tr("Insert && Cl&ose"))
        self.insertAndCloseButton_.setAccel(self.__tr("Alt+O"))
        self.cancelButton_.setText(self.__tr("&Close"))
        self.cancelButton_.setAccel(self.__tr("Alt+C"))
        self.treeView_.header().setLabel(0,self.__tr("Files"))
        self.extComboBox_.clear()
        self.extComboBox_.insertItem(self.__tr("*.xml;*.dita"))
        self.extComboBox_.insertItem(self.__tr("*"))
        self.listView_.header().setLabel(0,self.__tr("Select Target"))
        self.listView_.header().setLabel(1,self.__tr("Type"))
        self.filterComboBox_.clear()
        self.filterComboBox_.insertItem(self.__tr("All Targets"))
        self.filterComboBox_.insertItem(self.__tr("Topic"))
        self.filterComboBox_.insertItem(self.__tr("Step"))
        self.filterComboBox_.insertItem(self.__tr("List Item"))
        self.filterComboBox_.insertItem(self.__tr("Example"))
        self.filterComboBox_.insertItem(self.__tr("Section"))
        self.filterComboBox_.insertItem(self.__tr("Figure"))
        self.filterComboBox_.insertItem(self.__tr("Table"))


    def insertLink(self):
        print "InsertReferenceDialog.insertLink(): Not implemented yet"

    def browse(self):
        print "InsertReferenceDialog.browse(): Not implemented yet"

    def selectionChanged(self):
        print "InsertReferenceDialog.selectionChanged(): Not implemented yet"

    def fileOrFolderChanged(self):
        print "InsertReferenceDialog.fileOrFolderChanged(): Not implemented yet"

    def extensionChanged(self):
        print "InsertReferenceDialog.extensionChanged(): Not implemented yet"

    def filterChanged(self):
        print "InsertReferenceDialog.filterChanged(): Not implemented yet"

    def insertLinkAndClose(self):
        print "InsertReferenceDialog.insertLinkAndClose(): Not implemented yet"

    def __tr(self,s,c = None):
        return qApp.translate("InsertReferenceDialog",s,c)

if __name__ == "__main__":
    a = QApplication(sys.argv)
    QObject.connect(a,SIGNAL("lastWindowClosed()"),a,SLOT("quit()"))
    w = InsertReferenceDialog()
    a.setMainWidget(w)
    w.show()
    a.exec_loop()
