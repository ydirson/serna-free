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
import  SernaApiCore
from    SernaApiCore    import *
import  sys, sip

def plugin_path(plugin, path = None):
    tdir = Url(plugin.sernaDoc().getDsi().getSafeProperty("template-path").\
        getString()).upperUrl().asString()
    if path == None:
        return tdir
    return unicode(Url(tdir).combineDir2Path(Url(path)).asString()) 

#
# This class redirects Python stdout/stderr to the temporary section
# of the Serna config data where it can be accessed by the Python message
# viewer plugin.
#
class SernaPythonDefaultOutputStream:
    def __init__(self):
        messages  = SernaConfig.root().makeDescendant("#python-messages")
        self.seq_ = messages.makeDescendant("sequence", "1", False)
        self.message_ = messages.makeDescendant("message")

    def flush(self):
        return

    def write(self, s):
        self.message_.setString(s)
        self.seq_.setInt(self.seq_.getInt() + 1)

if sys.stdout == sys.__stdout__:
    sys.stdout = SernaPythonDefaultOutputStream()

if sys.stderr == sys.__stderr__:
    sys.stderr = sys.stdout

######################################################################

# Qt ui utilities

import  PyQt4, PyQt4.uic

def load_qt_uitype(base_path, ui_path):
    path = unicode(Url(base_path).combinePath2Path(Url(ui_path)).asString())
    return PyQt4.uic.loadUiType(path)

build_ui_widget = load_qt_uitype

def load_qt_widget(base_path, ui_path, baseinstance = None):
    path = unicode(Url(base_path).combinePath2Path(Url(ui_path)).asString())
    return PyQt4.uic.loadUi(path, baseinstance)
