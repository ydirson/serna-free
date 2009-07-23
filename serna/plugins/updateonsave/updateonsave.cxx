// 
// Copyright(c) 2009 Syntext, Inc. All Rights Reserved.
// Contact: info@syntext.com, http://www.syntext.com
// 
// This file is part of Syntext Serna XML Editor.
// 
// COMMERCIAL USAGE
// Licensees holding valid Syntext Serna commercial licenses may use this file
// in accordance with the Syntext Serna Commercial License Agreement provided
// with the software, or, alternatively, in accorance with the terms contained
// in a written agreement between you and Syntext, Inc.
// 
// GNU GENERAL PUBLIC LICENSE USAGE
// Alternatively, this file may be used under the terms of the GNU General 
// Public License versions 2.0 or 3.0 as published by the Free Software 
// Foundation and appearing in the file LICENSE.GPL included in the packaging 
// of this file. In addition, as a special exception, Syntext, Inc. gives you
// certain additional rights, which are described in the Syntext, Inc. GPL 
// Exception for Syntext Serna Free Edition, included in the file 
// GPL_EXCEPTION.txt in this package.
// 
// You should have received a copy of appropriate licenses along with this 
// package. If not, see <http://www.syntext.com/legal/>. If you are unsure
// which license is appropriate for your use, please contact the sales 
// department at sales@syntext.com.
// 
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
// 
#include "sapi/app/DocumentPlugin.h"
#include "sapi/app/SernaDoc.h"
#include "sapi/app/StructEditor.h"
#include "sapi/grove/Grove.h"
#include "sapi/grove/GroveIdManager.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/grove/GroveEditor.h"

#include <time.h>

#define NOTR(x) x

using namespace SernaApi;

// Plugin implementation class
class UpdateOnSavePlugin : public SernaApi::DocumentPlugin {
public:
    UpdateOnSavePlugin(SernaApiBase* ssd, SernaApiBase* properties, char**)
        : DocumentPlugin(ssd, properties) {}
    
    // This hook is called before 'save document' command.
    // Reimplemented from DocumentPlugin.
    virtual void aboutToSave();
};

SAPI_DEFINE_PLUGIN_CLASS(UpdateOnSavePlugin)

//////////////////////////////////////////////////////////////////////////

// Returns current date and time converted to string
static SString date_time_stamp()
{
   time_t long_time;
   (void) time(&long_time);
   return SString(asctime(localtime(&long_time)));
}

void UpdateOnSavePlugin::aboutToSave()
{
    // StructEditor is wysiwyg editor itself
    StructEditor se = sernaDoc().structEditor();
    GroveEditor  ge = se.groveEditor();

    // takes article root element
    GroveElement article = GroveElement(se.sourceGrove().document().
                                        documentElement().asGroveElement());
    
    // if this is not a Docbook article, exit
    if (article.isNull() || NOTR("article") != article.nodeName()) 
        return;
        
    // Try to find 'articleinfo' among 'article' children
    GroveNode n = article.firstChild();
    for (; n && NOTR("articleinfo") != n.nodeName(); n = n.nextSibling())
        ;
    // if 'articleinfo' is not found, warning is shown
    if (n.isNull()) {
        sernaDoc().showMessageBox(SernaDoc::MB_CRITICAL, 
            UiItem::translate("UpdateOnSavePlugin", "Error"), 
            UiItem::translate("UpdateOnSavePlugin", 
         "'articleinfo' is not found, \nmodification date will not be saved."),
            UiItem::translate("", "&OK"));
        return;
    }
    // Try to find date in 'articleinfo' children
    GroveNode articleinfo = n;
    n = articleinfo.firstChild();
    for (; n && NOTR("date") != n.nodeName(); n = n.nextSibling())
        ;
    // if there is no 'date' element - create 'date'
    // and fill 'date' element with text of date and time.
    if (n.isNull()) {
        GroveElement date_elem(NOTR("date"));
        date_elem.appendChild(GroveText(date_time_stamp()));
        
        // create document fragment with 'date' element. 
        GroveDocumentFragment df;
        df.appendChild(date_elem);
        
        // Insert this document fragment at the end of articleinfo
        se.executeAndUpdate(ge.paste(df, GrovePos(articleinfo))); 
        return;
    }
    // If 'date' element is already created, replace date text; if there
    // is no text, insert new text instead
    GroveText date_text = n.firstChild().asGroveText();
    if (date_text)
        se.executeAndUpdate(ge.replaceText(GrovePos(date_text),
            date_text.data().length(), date_time_stamp()));
    else
        se.executeAndUpdate(ge.insertText(GrovePos(n), date_time_stamp()));
}
