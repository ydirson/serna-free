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
from SernaApi import *
from SetAdpFileDialog import SetAdpFileDialog
from os.path import basename


# Root's attribute
ADP_FILE = "adp-file"


class DoubleClickWatcher(SimpleWatcher):
    """
        Class for handling mouse double-clicks
    """

    def __init__(self, plugin):
        SimpleWatcher.__init__(self)

        self.__plugin = ref(plugin)

    def notifyChanged(self):
        pos = self.__plugin().sernaDoc().structEditor().getCheckedPos()
        if pos.isNull():
            return True

        if pos.node().asGroveElement().localName() == "serna-help":
            self.__plugin().setAdpFile()
            return False

        return True


##############################################################
class ElementHelp(DocumentPlugin):
    """
        Plugin for working with ElementHelp document
    """

    def __init__(self, a1, a2):
        DocumentPlugin.__init__(self, a1, a2)
        self.buildPluginExecutors(True)

    def postInit(self):
        self.se = self.sernaDoc().structEditor()
        self.__watcher = DoubleClickWatcher(self)
        self.se.setDoubleClickWatcher(self.__watcher)

    def executeUiEvent(self, evName, cmd):
        if evName == "SetAdpFileDialogEvent":
			self.setAdpFile()

    def setAdpFile(self):
        root = self.se.sourceGrove().document() 
        root_attrs = root.documentElement().attrs()

        adp_fname_attr = root_attrs.getAttribute(ADP_FILE)
        adp_fname = str(adp_fname_attr.value())

        adp_fname = SetAdpFileDialog().getAdpFileName(adp_fname)
        if adp_fname:
            adp_fname = basename(str(adp_fname))

            if not adp_fname_attr:
                prop_item = PropertyNode(ADP_FILE, adp_fname)
                cmd = self.se.groveEditor().addAttribute(root.documentElement(), prop_item)
            else:
                cmd = self.se.groveEditor().setAttribute(adp_fname_attr, adp_fname)

            self.se.executeAndUpdate(cmd)
