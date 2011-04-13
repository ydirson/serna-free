// 
// Copyright(c) 2011 Syntext, Inc. All Rights Reserved.
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
#ifndef DOCVIEW_DOC_BUILDER_H_
#define DOCVIEW_DOC_BUILDER_H_

#include "docview/dv_defs.h"
#include "common/PropertyTree.h"

namespace Sui {
    class ActionDispatcher;
    class ActionSet;
}

class DOCVIEW_EXPIMP DocBuilder {
public:
    DocBuilder();
    virtual ~DocBuilder();

    virtual void    buildActions(Sui::ActionDispatcher* dispatcher,
                                 Sui::ActionSet* actionSet) = 0;

    virtual Common::PropertyNode* loadSui() = 0;

    /// special case for StructEditor - it is not created as normal UI item
    Common::PropertyNode* doceditorProps() const { return docEditorProps_; }

protected:
    void    load_sui(const Common::String& name);        
    void    makeActions(Sui::ActionSet*) const;

    Common::PropertyNodePtr suiProps_;
    Common::PropertyNode*   actionList_;
    Common::PropertyNode*   docEditorProps_;
};

#endif // DOCVIEW_DOC_BUILDER_H_
