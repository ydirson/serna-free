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

from PyQt4.QtGui            import QApplication, QMessageBox, QMainWindow, \
                                   QFileDialog
from PyQt4.QtCore           import QFileInfo, QUrl, Qt
from dialogs.PublishDialog  import PublishDialog
from PublishersFactory      import *
from Transformer            import *
from utils import dump_ptree, set_java_home

try:
    from SernaApi import *
except:
    from XSernaApi.DocumentPlugin import *

import os

__all__ = ['PublishingPlugin']

def init_factory(factory, FactoryClass, props=None):
    return factory

class PublishingPlugin(DocumentPlugin):
    __publishersRegistry = None
    __transformersFactory = None

    @staticmethod
    def getPublishersRegistry(props=None):
        if PublishingPlugin.__publishersRegistry is None:
            PublishingPlugin.__publishersRegistry = PublishersRegistry(props)
        return PublishingPlugin.__publishersRegistry

    @staticmethod
    def getTransformersFactory(props=None):
        if PublishingPlugin.__transformersFactory is None:
            PublishingPlugin.__transformersFactory = TransformersFactory(props)
        return PublishingPlugin.__transformersFactory

    def __init__(self, sernaDoc, properties):
        DocumentPlugin.__init__(self, sernaDoc, properties)
        self.buildPluginExecutors(True)
        pp = self.pluginProperties()
        self._pluginDir = unicode(pp.getProperty("resolved-path").getString())
        self._dialog = None
        config_root = SernaConfig.root()
        java_home = config_root.getProperty("vars/java_home").getString()
        set_java_home(unicode(java_home))

    def __str__(self):
        return PLUGIN_ID

    def launchViewer(self, url):
        urlProperty = PropertyNode("")
        urlProperty.makeDescendant("url", url)
        self.executeCommandEvent("LaunchBrowser", urlProperty)

    def executeUiEvent(self, evName, cmd):
        if "PublishCommandEvent" != evName:
            return

        sernaDoc = self.sernaDoc()
        structEditor = sernaDoc.structEditor()

        widget = sernaDoc.widget()

        if structEditor and not structEditor.groveEditor is None:
            saveAction = structEditor.findAction("saveDocument")
            if saveAction and saveAction.isEnabled():
                buttons = QMessageBox.Ok | QMessageBox.Cancel
                answer = QMessageBox.warning(widget, "Publishing", "Current "
                                             "document will be saved", buttons)
                if QMessageBox.Ok != answer:
                    return

                if not self.executeCommandEvent("SaveDocument"):
                    QMessageBox.critical(widget, "Publishing",
                                         "Document save failed")
                    return

        catMgr = structEditor.catalogManager()
        catalogFiles = unicode(catMgr.getCatalogsList())

        if os.pathsep in catalogFiles:
            cats = []
            for cat in catalogFiles.split(os.pathsep):
                cat = cat.replace(' ', '%20')
                cats.append(cat)
            catalogFiles = ' '.join(cats)

        sernaDoc.getDsi().getProperty("xml-catalogs").setString(catalogFiles)

        if not self._dialog:
            self._dialog = PublishDialog(self, widget, self.pluginProperties())

        self._dialog.showDialog()
        QApplication.setActiveWindow(self._dialog)
        self._dialog.setFocus(Qt.ActiveWindowFocusReason)
        self._dialog.exec_()
