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
#include "sapi/app/SernaDoc.h"
#include "sapi/app/StructEditor.h"
#include "sapi/app/MessageView.h"
#include "sapi/app/DocumentPlugin.h"
#include "sapi/app/MimeHandler.h"
#include "sapi/common/PropertyNode.h"
#include "sapi/grove/Grove.h"

#include "structeditor/StructEditor.h"

#include "docview/dv_utils.h"
#include "docview/SernaDoc.h"
#include "docview/MessageView.h"
#include "utils/HelpAssistant.h"

#include "ui/UiAction.h"
#include "ui/MainWindow.h"
#include "ui/UiItemSearch.h"
#include "ui/MimeHandler.h"
#include "common/PropertyTree.h"
#include "common/Url.h"

#define PN_IMPL(n) (static_cast<Common::PropertyNode*>(n.getRep()))
#define DOC_TH (static_cast<DocTemplate::DocTemplateHolder*>(getRep()))

namespace SernaApi {

typedef ::SernaDoc SD;

#define SELF (static_cast<SD*>(getRep()))

SernaDoc::SernaDoc(SernaApiBase* rep, DocumentPlugin* dp)
    : UiDocument(0), dp_(dp)
{
    SimpleWrappedObject::setRep(rep);
}
    
void SernaDoc::setRep(SernaApiBase* b)
{
    SimpleWrappedObject::setRep(b);
}
    
SernaDoc::SernaDoc(const SernaDoc& sd)
    : UiDocument(0), dp_(sd.dp_)
{
    SimpleWrappedObject::setRep(sd.getRep());
}

SernaDoc& SernaDoc::operator=(const SernaDoc& sd)
{
    SimpleWrappedObject::setRep(sd.getRep());
    dp_ = sd.dp_;
    return *this;
}

///////////////////////////////////////////////////////////////////////

static Common::PropertyNode* 
find_prop(Common::PropertyNode* n, const Common::String& name)
{
    Common::PropertyNode* pn = n->getProperty("properties/name");
    if (pn && pn->getString() == name)
        return n;
    for (n = n->firstChild(); n; n = n->nextSibling()) {
        if (n->name() == "properties")
            continue;
        pn = find_prop(n, name);
        if (pn)
            return pn;
    }
    return 0;
}

UiItem SernaDoc::buildUiItem(const PropertyNode& uiTree,
                             const SString& childName) const
{
    if (uiTree.isNull())
        return 0;
    Common::PropertyNode* pn = 
        static_cast<Common::PropertyNode*>(uiTree.getRep());
    if (childName.isNull())
        return restore_ui_item(SELF, pn, 0);
    pn = find_prop(pn, childName);
    return pn ? restore_ui_item(SELF, pn, 0) : 0;
}

int SernaDoc::showMessageBox(MessageBoxSeverity severity,
                             const SString& caption,
                             const SString& message,
                             const SString& button0,
                             const SString& button1,
                             const SString& button2) const
{
    if (!getRep())
        return -1;
    return SELF->showMessageBox((SD::MessageBoxSeverity) severity,
        caption, message, button0, button1, button2);
}

PropertyNode SernaDoc::getDsi() const
{
    return PropertyNode(SELF->getDsi());
}

StructEditor SernaDoc::structEditor() const
{
    typedef ::StructEditor SE;
    SE* s = dynamic_cast<SE*>(SELF->findItem(Sui::ItemClass(Sui::STRUCT_EDITOR)));
    return StructEditor(s, dp_);
}

typedef ::MessageView MV;

static MV* message_view(const Sui::Item* item)
{
    return dynamic_cast<MV*>(item->findItem(Sui::ItemClass(Sui::MESSAGE_VIEW)));
}

MessageView SernaDoc::messageView() const
{
    return MessageView(message_view(SELF), dp_);
}
    
void SernaDoc::showHelp(const SString& ref, 
                        const SString& adp) const
{
    Common::String spd_adp = dp_->pluginProperties().
        getSafeProperty("help-assistant-profile").getString();
    Common::String adp_path = dp_->pluginProperties().
        getSafeProperty("resolved-path").getString();
    if (adp.isEmpty()) {
        if (spd_adp.isEmpty())
            return helpAssistant().show(ref, adp);
        Common::Url spd_adp_url(spd_adp);
        if (spd_adp_url.isRelative())
            adp_path = Common::Url(adp_path).combineDir2Path(spd_adp_url);
        else
            adp_path = spd_adp;
    } else {
        Common::Url adp_url(adp);
        if (adp_url.isRelative())
            adp_path = Common::Url(adp_path).combineDir2Path(adp_url);
        else
            adp_path = adp_url;
    }
    helpAssistant().show(ref, adp_path);
}

SernaDoc SernaDoc::activeDocument()
{
    return ::SernaDoc::activeDocument();
}

void SernaDoc::setActive()
{
    if (!getRep() || getRep() == activeDocument().getRep())
        return;
    Sui::MainWindow* main_win = dynamic_cast<Sui::MainWindow*>(SELF->parent());
    if (main_win)
        main_win->setCurrent(SELF);
}

MimeHandler SernaDoc::mimeHandler() const
{
    return MimeHandler(&SELF->mimeHandler());
}

DocumentPlugin* SernaDoc::plugin() const
{
    return dp_;
}

SernaDoc::~SernaDoc()
{
    releaseRep();
}

/////////////////////////////////////////////////////////////////

Grove SernaDoc::groveFromTemplate(const PropertyNode& docTemplate,
                                  const SString& url,
                                  const SString& skel) const
{
    if (!getRep() || !docTemplate.getRep())
        return Grove();
    ::SourceDocument srd(SELF);    
    if (!srd.createGroveFromTemplate(
        static_cast<Common::PropertyNode*>(docTemplate.getRep()), url, skel))
            return Grove();
    return srd.grove();
}

} // namespace SernaApi
