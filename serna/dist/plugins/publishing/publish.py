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
from PyQt4.QtCore           import QFileInfo, QUrl
from PublishingPlugin       import PublishingPlugin

from XSernaApi              import *

import os

dataDir = os.environ.get('SERNA_DATA_DIR', '')

SernaConfig.SernaConfig.init(**{'vars/dita_ot_dir': \
        os.path.join(dataDir, r"plugins\dita\DITA-OT1.4")})

from publishers import DocbookPublisher, DitaPublisher

DocbookPublisher.register_creators(dataDir + 'plugins/docbook')
DitaPublisher.register_creators(dataDir + 'plugins/dita')

import sys, getopt
import sip

xsd = xslt = catalogs = None
propFile = dsiFile = None

if __name__ == "__main__":

    app = QApplication(sys.argv)

    opts, args = getopt.gnu_getopt(sys.argv[1:], "s:x:c:p:d:",
                                   ['schema=', 'style=', 'catalogs='
                                    'pp=', 'dsi='])

    for o, a in opts:
        if o in ('-s', '--schema'):
            xsd = a.strip(' "\'')
        elif o in ('-x', '--style'):
            xslt = a.strip(' "\'')
        elif o in ('-c', '--catalogs'):
            catalogs = a.strip(' "\'')
        elif o in ('-p', '--pp'):
            propFile = a.strip(' "\'')
        elif o in ('-d', '--dsi'):
            dsiFile = a.strip(' "\'')

    if propFile and dsiFile:
        pp = PropertyNode.read_property_tree(propFile)
        dsi = PropertyNode.read_property_tree(dsiFile)
        sernaDoc = SernaDoc.SernaDoc(dsi)

        publishingPlugin = PublishingPlugin(sernaDoc, pp)
        publishingPlugin.executeUiEvent("PublishCommandEvent", None)

    sys.exit(0)

