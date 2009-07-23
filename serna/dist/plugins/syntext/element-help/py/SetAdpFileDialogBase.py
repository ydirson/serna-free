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

# Form implementation generated from reading ui file 'SetAdpFileDialogBase.ui'
#
# Created: Thu May  8 15:16:11 2008
#      by: PyQt4 UI code generator 4.3.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_SetAdpFileDialogBase(object):
    def setupUi(self, SetAdpFileDialogBase):
        SetAdpFileDialogBase.setObjectName("SetAdpFileDialogBase")
        SetAdpFileDialogBase.resize(QtCore.QSize(QtCore.QRect(0,0,426,100).size()).expandedTo(SetAdpFileDialogBase.minimumSizeHint()))

        self.vboxlayout = QtGui.QVBoxLayout(SetAdpFileDialogBase)
        self.vboxlayout.setObjectName("vboxlayout")

        self.hboxlayout = QtGui.QHBoxLayout()
        self.hboxlayout.setObjectName("hboxlayout")

        self.filenameLabel_ = QtGui.QLabel(SetAdpFileDialogBase)
        self.filenameLabel_.setObjectName("filenameLabel_")
        self.hboxlayout.addWidget(self.filenameLabel_)

        self.filenameLineEdit_ = QtGui.QLineEdit(SetAdpFileDialogBase)
        self.filenameLineEdit_.setObjectName("filenameLineEdit_")
        self.hboxlayout.addWidget(self.filenameLineEdit_)

        self.browseButton_ = QtGui.QPushButton(SetAdpFileDialogBase)
        self.browseButton_.setObjectName("browseButton_")
        self.hboxlayout.addWidget(self.browseButton_)
        self.vboxlayout.addLayout(self.hboxlayout)

        self.hboxlayout1 = QtGui.QHBoxLayout()
        self.hboxlayout1.setObjectName("hboxlayout1")

        spacerItem = QtGui.QSpacerItem(201,20,QtGui.QSizePolicy.Expanding,QtGui.QSizePolicy.Minimum)
        self.hboxlayout1.addItem(spacerItem)

        self.okButton_ = QtGui.QPushButton(SetAdpFileDialogBase)
        self.okButton_.setObjectName("okButton_")
        self.hboxlayout1.addWidget(self.okButton_)

        self.cancelButton_ = QtGui.QPushButton(SetAdpFileDialogBase)
        self.cancelButton_.setObjectName("cancelButton_")
        self.hboxlayout1.addWidget(self.cancelButton_)
        self.vboxlayout.addLayout(self.hboxlayout1)
        self.filenameLabel_.setBuddy(self.filenameLineEdit_)

        self.retranslateUi(SetAdpFileDialogBase)
        QtCore.QMetaObject.connectSlotsByName(SetAdpFileDialogBase)

    def retranslateUi(self, SetAdpFileDialogBase):
        SetAdpFileDialogBase.setWindowTitle(QtGui.QApplication.translate("SetAdpFileDialogBase", "Dialog", None, QtGui.QApplication.UnicodeUTF8))
        self.filenameLabel_.setText(QtGui.QApplication.translate("SetAdpFileDialogBase", "&Filename:", None, QtGui.QApplication.UnicodeUTF8))
        self.browseButton_.setText(QtGui.QApplication.translate("SetAdpFileDialogBase", "&Browse...", None, QtGui.QApplication.UnicodeUTF8))
        self.okButton_.setText(QtGui.QApplication.translate("SetAdpFileDialogBase", "&OK", None, QtGui.QApplication.UnicodeUTF8))
        self.cancelButton_.setText(QtGui.QApplication.translate("SetAdpFileDialogBase", "&Cancel", None, QtGui.QApplication.UnicodeUTF8))

