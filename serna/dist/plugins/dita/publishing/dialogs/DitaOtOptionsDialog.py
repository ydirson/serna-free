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
import os, sys

from ui             import build_ui_widgets
from PyQt4.QtCore   import Qt, QFileInfo
from PyQt4.QtGui    import QApplication, QMessageBox, QFileDialog

Ui_OptionsDialog, DialogBase = build_ui_widgets("DitaOtOptionsDialog")

class DitaOtOptionsDialog(Ui_OptionsDialog, DialogBase):
    def __init__(self, publisher, parentWidget=None):
        DialogBase.__init__(self, parentWidget)
        self.setupUi(self)
        self.setObjectName("DitaOtOptionsDialog")
        # This flag is needed for correct non-modality working in MacOSX
        flags = self.windowFlags() | Qt.Tool
        self.setWindowFlags(flags)
        self._propMap = { 'antScript': { \
                             'edit': self.antScriptLineEdit_,
                             'view': self.antScriptLabel_ },
                          'antMakefile':  { \
                             'edit': self.antMakefileLineEdit_,
                             'view': self.antMakefileLabel_ },
                          'antOptions': { \
                             'edit': self.antOptionsLineEdit_ ,
                             'view': self.antOptionsLabel_ } }

    def getProperty(self, name):
        prop = self._propMap.get(name, None)
        if prop:
            return unicode(prop['edit'].text())
        return None

    def setLabel(self, name, labelText):
        prop = self._propMap.get(name, None)
        if prop:
            prop['view'].setText(unicode(labelText))

    def on_applyButton__clicked(self, released=True):
        if not released:
            return
        self.accept()

    def on_cancelButton__clicked(self, released=True):
        if not released:
            return
        self.reject()

    def _onBrowseForClicked(self, lineEdit, title, filterString):
        text = QFileDialog.getOpenFileName(self, title,
                                           lineEdit.text(), filterString)

        if text:
            lineEdit.setText(text)

        QApplication.setActiveWindow(self)
        self.setFocus(Qt.ActiveWindowFocusReason)

        return text

    def on_browseForAntScriptButton__clicked(self, released=True):
        if not released:
            return

        if 'win32' == sys.platform:
            exeFiles = "Program files (*.cmd *.bat *.exe);;"
        else:
            exeFiles = "Shell scripts (*.sh);;"

        self._onBrowseForClicked(self.antScriptLineEdit_,
                                 "Choose Ant script file",
                                 exeFiles + "All files (*)")

    def on_browseForAntMakefileButton__clicked(self, released=True):
        if not released:
            return

        self._onBrowseForClicked(self.antMakefileLineEdit_,
                                 "Choose Ant makefile",
                                 "XML files (*.xml);;All files (*)")

def get_dita_options(publisher, widget):
    dlg = DitaOtOptionsDialog(publisher, widget)

    if publisher._antScript:
        dlg.antScriptLineEdit_.setText(publisher._antScript)

    if publisher._antMakefile:
        dlg.antMakefileLineEdit_.setText(publisher._antMakefile)

    if publisher._antOptions:
        dlg.antOptionsLineEdit_.setText(publisher._antOptions)

    dlg.show()
    QApplication.setActiveWindow(dlg)
    dlg.setFocus(Qt.ActiveWindowFocusReason)

    return (dlg.Accepted == dlg.exec_()), dlg
