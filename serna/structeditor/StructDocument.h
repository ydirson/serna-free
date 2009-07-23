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
#ifndef STRUCT_DOCUMENT_H_
#define STRUCT_DOCUMENT_H_

#include "common/CommandEvent.h"
#include "common/PropertyTree.h"
#include "docview/SernaDoc.h"
#include "docview/MessageViewHolder.h"
#include "structeditor/se_defs.h"
#include "utils/SernaUiItems.h"

namespace Sui {
    class Splitter;
}

class StructEditor;
class MessageView;

/*
 */
class STRUCTEDITOR_EXPIMP StructDocument : public SernaDoc,
                                           public MessageViewHolder {
public:
    typedef COMMON_NS::PropertyNode Ptn;

    StructDocument(const COMMON_NS::CommandEventPtr& event,
                   const COMMON_NS::PropertyNode* dsi,
                   Sui::Item* prevDoc, const DocBuilder* builder);
    virtual ~StructDocument();

    virtual bool        canCloseDocument() const;
    virtual void        grabFocus() const;

    virtual Item*       findItem(const Sui::ItemPred& pred) const;
    virtual String      itemClass() const { return Sui::STRUCT_DOCUMENT; }

    StructEditor*       structEditor() const { return structEditor_.pointer(); }
    bool                hasXmlErrors() const;

    virtual void        updateTooltip();
    virtual bool        restoreView(const Common::String& restoreFrom);
    virtual Common::String  getLevelFile(Level level,
                                         Common::String* comment) const;

protected:
    virtual Sui::Item*   makeItem(const COMMON_NS::String& itemClass,
                                 Common::PropertyNode* properties) const;

    virtual Common::CommandEventPtr makeSaveEvent(Common::PropertyNode*) const;
    virtual Common::CommandEventPtr makeUiEventExecutor(ActionExecutor*);

private:
    Common::RefCntPtr<StructEditor>  structEditor_;
};

#endif
