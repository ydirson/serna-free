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
from utils          import dump_ptree

from dialogs.ProgressDialog import ProgressDialog

Ui_PublishDialog, DialogBase = build_ui_widgets("PublishDialog")

class PublishDialog(Ui_PublishDialog, DialogBase):
    def __init__(self, plugin, parentWidget=None, props=None):
        DialogBase.__init__(self, parentWidget)
        self.setupUi(self)
        self.setObjectName("PublishDialog")

        self._plugin = plugin
        self._props = props

        self._publishers = {}

        sernaDoc = self._plugin.sernaDoc()
        self._dsi = sernaDoc.getDsi()

        registry = self._plugin.getPublishersRegistry()
        self._factory = registry.getFactoryForDsi(self._dsi)

#        registry.dump()
#        self._factory.dump()

        tags = self._factory.getPublishersTags()
        tags.sort()
        for tag in tags:
            self.outputTypeCombo_.addItem(tag)
            
    def _getSrcPath(self):
        sernaDoc = self._plugin.sernaDoc()
        dsi = sernaDoc.getDsi()
        srcPath = dsi.getProperty("doc-src").getString()
        return unicode(QFileInfo(unicode(srcPath)).absoluteFilePath())

    def showDialog(self):

        curPublisher = self._getPublisher()
        hasPublishers = not curPublisher is None
        self.publishButton_.setEnabled(hasPublishers)
        self.browseToButton_.setEnabled(hasPublishers)
        self.advancedButton_.setEnabled(hasPublishers and \
                                        curPublisher.hasAdvancedOptions())

        if hasPublishers:
            self.setOutputFilePath()

    def _getPublisher(self):
        curIdx = self.outputTypeCombo_.currentIndex()
        if -1 != curIdx:
            publisher = None
            curTag = unicode(self.outputTypeCombo_.itemText(curIdx))
            if curTag in self._publishers:
                return self._publishers[curTag]
            try:
                publisher = self._factory.make(curTag, dsi=self._dsi)
                self._publishers[curTag] = publisher
            except:
                pass
            return publisher
        return None

    def _makeOutputFilePath(self):
        srcPath = self._getSrcPath()
        name, ext = os.path.splitext(srcPath)
        curPublisher = self._getPublisher()
        ext = curPublisher['extension']
        if not ext:
            ext = '.out'
        elif not ext.startswith('.'):
            ext = '.' + ext
        filepath = u"%s%s" % (name, ext)
        if os.path.exists(filepath):
            os.unlink(filepath)
        return filepath

    def setOutputFilePath(self):
        self.outputFileEdit_.setText(self._makeOutputFilePath())

    def publishComplete(self, exitCode, crashed):
        pass

    def on_outputTypeCombo__currentIndexChanged(self):
        curPublisher = self._getPublisher()
        if not curPublisher:
            return
        self.advancedButton_.setEnabled(curPublisher and \
                                        curPublisher.hasAdvancedOptions())
        self.setOutputFilePath()

    def on_publishButton__clicked(self, released=True):
        if not released:
            return
        curPublisher = self._getPublisher()
        outFile = unicode(self.outputFileEdit_.text())
        progressDialog = ProgressDialog(curPublisher, self._plugin, self)
        result = progressDialog.publish(self._dsi, outFile)

    def on_advancedButton__clicked(self, released=True):
        if not released:
            return

        curPublisher = self._getPublisher()
        if curPublisher:
            curPublisher.fillAdvancedOptions(self)

    def on_cancelButton__clicked(self, released=True):
        if not released:
            return
        self.reject()

    def on_browseToButton__clicked(self, released=True):
        if not released:
            return

        curPublisher = self._getPublisher()
        ext = curPublisher['extension']
        filters = []
        if ext:
            inscription = curPublisher['inscription']
            if inscription is None:
                inscription = '%s Files' % ext.upper()
            if not ext.startswith('.'):
                ext = '.' + ext
            extFilter = u"%s (*%s)" % (inscription, ext)
            filters.append(extFilter)
        filters.append(u"All Files (*)")

        outputFile = self._makeOutputFilePath()
        outputFile = QFileDialog.getSaveFileName(self,
                                                 "Choose output file",
                                                 outputFile,
                                                 ';;'.join(filters))
        if outputFile:
            self.outputFileEdit_.setText(outputFile)

