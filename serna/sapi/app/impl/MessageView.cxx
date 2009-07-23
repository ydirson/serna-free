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
#include "sapi/app/MessageView.h"
#include "sapi/app/DocumentPlugin.h"
#include "sapi/grove/GroveNodes.h"
#include "docview/MessageView.h"
#include "grove/Node.h"

typedef ::MessageView SernaMessageView;

#define MV_SELF static_cast<SernaMessageView*>(getRep())

namespace SernaApi {

MessageView::MessageView(SernaApiBase* rep, DocumentPlugin* dp)
    : UiItem(0), dp_(dp) 
{
    SimpleWrappedObject::setRep(rep);
}

void MessageView::emitMessage(const SString& msg, const GroveNode* n)
{
    if (getRep()) {
        MV_SELF->emitPluginMessage(dp_->pluginName(),
            msg, n ? static_cast<GroveLib::Node*>(n->getRep()) : 0);
        MV_SELF->update();
    }
}

void MessageView::clearMessages(int facility)
{
    if (!getRep()) 
        return;
    if (facility >= 0)
        MV_SELF->messageTree()->clearMessagesFrom(facility);
    else
        MV_SELF->messageTree()->removeAllChildren();            
    MV_SELF->update();
}

void MessageView::setRep(SernaApiBase* ref)
{
    SimpleWrappedObject::setRep(ref);
}

MessageView::MessageView(const MessageView& mv)
    : UiItem(0)
{
    (void) operator=(mv);
}

MessageView& MessageView::operator=(const MessageView& mv)
{
    SimpleWrappedObject::setRep(mv.getRep());
    dp_ = mv.dp_;
    return *this;
}

MessageView::~MessageView()
{
    SimpleWrappedObject::setRep(0);
}

} // namespace SernaApi
