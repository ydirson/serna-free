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
#ifndef CC_XPATHUTILS_H_
#define CC_XPATHUTILS_H_

#include "xslt/ResultOrigin.h"
#include "xslt/PatternExpr.h"
#include "xpath/ConstValueHolder.h"
#include "xpath/ExprContext.h"
#include "xpath/NodeSet.h"
#include "xpath/ExternalFunction.h"
#include "grove/Nodes.h"
#include "groveeditor/GrovePos.h"
#include "common/Exception.h"

class CustomContext;

class NodesFunction : public Xpath::ExternalFunctionBase {
public:
    NodesFunction(const CustomContext* ctx, bool isAfter)
        : ctx_(ctx), isAfter_(isAfter) {}
    virtual Xpath::ValueHolder* eval(const Xpath::ConstValuePtrList&) const;
    virtual ~NodesFunction() {}

private:
    const CustomContext* ctx_;
    bool                 isAfter_;
};

class CustomContext : public Xpath::ExprContext {
public:
    CustomContext(const GroveEditor::GrovePos& pos);
    virtual ~CustomContext();

    virtual Common::String contextString() const { return ""; }
    virtual void    registerVisitor(const GroveLib::Node*, 
                                    GroveLib::NodeVisitor*, short) const {}
    virtual const Xpath::ExternalFunctionBase* getExternalFunction
        (const Common::String& name, const Common::String& /* nsUri */) const;
    
    const GroveEditor::GrovePos&  pos() const { return pos_; }
    const GroveLib::NodeWithNamespace* ns_node() const 
        { return resolver_.pointer(); }
    bool  isValid() const { return !expr_.isNull(); }

protected:
    GroveLib::ElementPtr    resolver_;
    GroveEditor::GrovePos   pos_;
    NodesFunction           nodesBeforeFunc_;
    NodesFunction           nodesAfterFunc_;
    Xpath::ExprPtr          expr_;
};

class XpathLocator : public CustomContext {
public:
    XpathLocator(const Common::String& str, const GroveEditor::GrovePos& pos);
    GroveLib::Node*   locate() const;
};

class PatternMatcher : public CustomContext {
public:
    PatternMatcher(const Common::String& str, const GroveEditor::GrovePos& pos);
    bool    matches() const; 
};

#endif // CC_XPATHUTILS_H_
