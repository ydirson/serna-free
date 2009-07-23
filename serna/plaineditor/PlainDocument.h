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
#ifndef PLAIN_DOCUMENT_H_
#define PLAIN_DOCUMENT_H_

#include "common/CommandEvent.h"
#include "common/PropertyTree.h"
#include "docview/SernaDoc.h"
#include "docview/MessageTreeHolder.h"
#include "docview/MessageViewHolder.h"
#include "plaineditor/pe_defs.h"
#include "utils/SernaUiItems.h"

class PlainEditor;
class MessageView;

class PLAINEDITOR_EXPIMP PlainDocument : public SernaDoc,
                                         public MessageTreeHolder,
                                         public MessageViewHolder {
public:
    PlainDocument(Common::PropertyNode* dsi, 
                  MessageTreeNode* mtn,
                  const DocBuilder* builder);
    virtual ~PlainDocument();

    virtual String      itemClass() const { return Sui::PLAIN_DOCUMENT; }

    virtual bool        canCloseDocument() const;
    virtual void        updateTooltip();

    virtual void        grabFocus() const;
    virtual Common::String  getLevelFile(Level level,
                                         Common::String* comment) const;

protected:
    virtual Sui::Item*   makeItem(const COMMON_NS::String& itemClass,
                                 Common::PropertyNode* properties) const;

    virtual Common::CommandEventPtr makeSaveEvent(Common::PropertyNode*) const;
    virtual Common::CommandEventPtr makeUiEventExecutor(ActionExecutor*);

private:
    Common::RefCntPtr<PlainEditor>   plainEditor_;
};

#endif // PLAIN_DOCUMENT_H_
