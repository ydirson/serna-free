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
#ifndef XSLT_TOP_STYLESHEET_H
#define XSLT_TOP_STYLESHEET_H

#ifdef _MSC_VER
# pragma warning( disable : 4786 )
#endif

#include "xslt/xslt_defs.h"
#include "grove/Decls.h"
#include "xslt/impl/instructions/Stylesheet.h"
#include "xslt/impl/instructions/ApplicatingInstruction.h"

namespace Common {
    class MessageStream;
    class PropertyNode;
}

namespace Xslt {

class DocumentHolder;
class KeyProvider;
class Variable;
class AttributeSet;
class UseAttributeSet;
class Instance;
class CallTemplate;
class XsltResource;
class SharedData;
class BuiltinTemplateProvider;
class WhitespaceStripper;
class FunctionRegistry;
class XsltFunctionFactory;

class TopStylesheet : public Stylesheet,
                      public ApplicatingInstruction {
public:
    XSLT_OALLOC(TopStylesheet);

    TopStylesheet(const GroveLib::Element* element, XsltResource* res);
    virtual ~TopStylesheet();

    //! Returns associated document holder
    DocumentHolder*         docHolder() const { return docHolder_.pointer(); }

    //! Creates the top-level stylesheet to process given source document
    Instance*               makeInst(Common::MessageStream* ms,
                                     Common::MessageStream* ps,
                                     const GroveLib::GrovePtr& source,
                                     SharedData*,
                                     FunctionRegistry*) const;

    //! Combine stylesheets - flatten includes and build import chain
    void                    combineStylesheets();

    const Variable*         getTopVariable(const Common::String&) const;

    const AttributeSet*     getAttributeSet(const Common::String&) const;

    void                    getTopParamList(Common::PropertyNode*) const;

    void    registerAttributeSetUse(const UseAttributeSet* u) const;
    void    registerCallTemplate(CallTemplate* c) const;

    const BuiltinTemplateProvider& builtinTemplates() const
    {
        return *builtinTemplates_;
    }
    WhitespaceStripper* whitespaceStripper() const 
    {
        return whitespaceStripper_.pointer();
    }
    XsltFunctionFactory* functionFactory() const { return &*functionFactory_; }

private:
    TopStylesheet(const TopStylesheet&);

    class TopVariableMap;
    class AttributeSetMap;
    class UseAttributeSetList;
    class CallTemplateList;

    void    buildVariables();
    void    buildAttributeSets();
    void    dump_variables() const;

    Common::OwnerPtr<DocumentHolder>  docHolder_;
    Common::OwnerPtr<TopVariableMap>  topVarMap_;
    Common::OwnerPtr<AttributeSetMap> attrSetMap_;
    Common::OwnerPtr<UseAttributeSetList> usl_;
    Common::OwnerPtr<CallTemplateList> callTemplateList_;
    Common::OwnerPtr<BuiltinTemplateProvider> builtinTemplates_;
    Common::OwnerPtr<WhitespaceStripper> whitespaceStripper_;
    Common::OwnerPtr<XsltFunctionFactory> functionFactory_;
};

} // namespace Xslt

#endif // XSLT_TOP_STYLESHEET_H
