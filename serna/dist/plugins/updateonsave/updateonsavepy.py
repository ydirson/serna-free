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
# This plugin automatically updates Docbook articleinfo on save event.
#
from time import gmtime, strftime
from SernaApi import *

class UpdateOnSavePlugin(DocumentPlugin):
    def __init__(self, a1, a2): 
        DocumentPlugin.__init__(self, a1, a2)
        self.buildPluginExecutors()

    # This hook is called just before 'save document' command.
    # Reimplemented from DocumentPlugin.
    def aboutToSave(self):
        # StructEditor is wysiwyg editor itself
        se = self.sernaDoc().structEditor()
        ge = se.groveEditor()
        # take article root element
        article = GroveElement(se.sourceGrove().document().\
                               documentElement().asGroveElement())
        # if this is not a Docbook article, return
        if article.isNull() or "article" != article.nodeName(): 
            return
        # Try to find 'articleinfo' among 'article' children
        articleinfo = 0
        for n in article.children():
            if n.nodeName() == "articleinfo":
                articleinfo = n
                break
        # if 'articleinfo' is not found, warning is shown
        if 0 == articleinfo:
            self.sernaDoc().showMessageBox(2, "Error", \
                            "'articleinfo' not found, \nmodification "  \
                            "date will not be saved." , "&OK");
            return
        # Try to find date in 'articleinfo'children
        date = 0
        for n in articleinfo.children():
            if n.nodeName() == "date":
                date = n
                break
        # if there is no 'date' element - create 'date'
        # and fill 'date' element with text of date and time.
        if 0 == date:
            elem = GroveElement("date")
            elem.appendChild(GroveText(date_time_stamp()))
            # create document fragment with 'date' element. 
            df = GroveDocumentFragment()
            df.appendChild(elem)
            # Insert this document fragment at the end of articleinfo
            se.executeAndUpdate(ge.paste(df, GrovePos(articleinfo)))          
            return;
        # If 'date' element is already created, replace date text; if there
        # is no text, insert new text instead
        date_text = date.firstChild().asGroveText();
        if date_text:
            se.executeAndUpdate(ge.replaceText(GrovePos(date_text),\
                date_text.data().length(), date_time_stamp()))
        else:
            se.executeAndUpdate(ge.insertText(GrovePos(n), date_time_stamp()))

# Function returns current date and time stamp
def date_time_stamp():
   return SString(strftime("%d %b %Y %H:%M:%S", gmtime()))
