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
from utils import Value
from PropertyNode import PropertyNode
from SernaConfig import SernaConfig

class CatalogManager(Value):
    def __init__(self, **kwargs):
        Value.__init__(self, **kwargs)

    def getCatalogsList(self):
        return self._catalogsList

class StructEditor(Value):
    def __init__(self, **kwargs):
        Value.__init__(self, **kwargs)

    def catalogManager(self):
        return self._catalogManager

class SernaDoc(Value):
    def __init__(self, dsi, widget=None):
        Value.__init__(self, _dsi=dsi, _widget=widget)

        templateDir = dsi.getProperty("template_dir")
        templateDir = unicode(templateDir.getString())
        templateProp = PropertyNode()
        templateProp.makeDescendant("SERNA_TEMPLATE_DIR", templateDir)

        catalogs = dsi.getProperty("xml-catalogs")
        catalogsList = []
        for catalog in unicode(catalogs.getString()).split():
            catalog = SernaConfig.resolveResource('', catalog, '', templateProp)
            catalogsList.append("file:///" + catalog.replace(' ', '%20'))

        catalogs = ' '.join(catalogsList)
        cm = CatalogManager(_catalogsList=catalogs)
        self._structEditor = StructEditor(_catalogManager=cm)

    def getDsi(self):
        return self._dsi

    def structEditor(self):
        return self._structEditor

    def widget(self):
        return self._widget

