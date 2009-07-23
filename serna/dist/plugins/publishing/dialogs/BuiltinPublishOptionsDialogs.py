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
from ui             import build_ui_widgets
from PyQt4.QtCore   import Qt
from PyQt4.QtGui    import QApplication, QFileDialog

Ui_SimplePublishOptionsDialog, \
    SimpleDialogBase = build_ui_widgets("SimplePublishOptionsDialog.ui")

class SimplePublishOptionsDialog(Ui_SimplePublishOptionsDialog,
                                 SimpleDialogBase):
    def __init__(self, publisher, parentWidget=None):
        SimpleDialogBase.__init__(self, parentWidget)
        self.setupUi(self)
        self.setObjectName("SimplePublishOptionsDialog")
        self.scriptLineEdit_.setText(publisher.script)
        self.xslLineEdit_.setText(publisher.stylesheet)

    def getProperty(self, prop):
        if 'script' == prop:
            return unicode(self.scriptLineEdit_.text())
        elif 'stylesheet' == prop:
            return unicode(self.xslLineEdit_.text())
        return None

    def on_applyButton__clicked(self, released=True):
        if not released:
            return
        self.accept()

    def on_cancelButton__clicked(self, released=True):
        if not released:
            return
        self.reject()

    def on_browseForXslButton__clicked(self, released=True):
        if not released:
            return
        xslScript = self.xslLineEdit_.text()
        xslScript = QFileDialog.getOpenFileName(self,
                                                "Choose XSL script file",
                                                xslScript,
                                                "XSL files (*.xsl);;" \
                                                "All files (*)")
        if xslScript:
            self.xslLineEdit_.setText(xslScript)
        QApplication.setActiveWindow(self)
        self.setFocus(Qt.ActiveWindowFocusReason)

    def on_browseForScriptButton__clicked(self, released=True):
        if not released:
            return
        script = self.scriptLineEdit_.text()
        script = QFileDialog.getOpenFileName(self,
                                             "Choose script file",
                                             script, "All files (*)")
        if script:
            self.scriptLineEdit_.setText(script)
        QApplication.setActiveWindow(self)
        self.setFocus(Qt.ActiveWindowFocusReason)

