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
#ifndef XPATH_EXPR_CONTEXT_H
#define XPATH_EXPR_CONTEXT_H

#include "xpath/xpath_defs.h"
#include "xpath/VarBindings.h"
#include "common/StringDecl.h"

namespace GroveLib {
    class Node;
    class NodeVisitor;
};

namespace Common {
    class SubscriberPtrPublisher;
}

namespace Xpath {

class ExternalFunctionBase;

/*! ExprContext defines expression-specific context (variable bindings
 *  and namespace resolution scope).
 */
class XPATH_EXPIMP ExprContext : public VarBindings {
public:
    XPATH_OALLOC(ExprContext);

    virtual ValueHolder* getVariable(const Common::String&) const
        { return 0; }

    virtual Common::String contextString() const
        { return Common::String::null(); }

    /// Defines a way to attach node-visitors. Clients may redefine
    /// this to change behaviour of registering node visitors by axis exprs
    /// and conversion functions.
    /// NOTE: 3FF == GroveVisitor::NOTIFY_ALL
    virtual void    registerVisitor(const GroveLib::Node* node,
                                    GroveLib::NodeVisitor* visitor,
                                    short mask = 0x3FF) const;
    /// Register watcher for the expression. If not defined, must return
    /// false - then built-in watcher will be used.
    virtual bool    watchFor(Common::SubscriberPtrPublisher*) const 
        { return false; }
    
    /// Hook to external functions
    virtual const ExternalFunctionBase* getExternalFunction
        (const Common::String& /*name*/, const Common::String& /*nsUri*/) const
        { return 0; } 
};

} // namespace Xpath

#endif // XPATH_EXPR_CONTEXT_H
