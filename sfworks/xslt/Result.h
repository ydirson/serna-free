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
/*! \file
 */

#ifndef XSLT_RESULT_H
#define XSLT_RESULT_H

#include "xslt/xslt_defs.h"
#include "grove/Nodes.h"
#include "common/prtti.h"

COMMON_NS_BEGIN
class TreelocRep;
COMMON_NS_END

namespace Xslt {

class Instance;

//
// Base class for nodes in result tree. Note that it does not belong
// to XSLT namespace.
//
class XSLT_EXPIMP XsltResult {
public:
    XSLT_OALLOC(Result);

    XsltResult(const GroveLib::Node* node, const Xslt::Instance* i)
        : origin_(node), userPtr_(0), instance_(i) {}

    virtual ~XsltResult() {}

    virtual GroveLib::Node* node() = 0;

    const GroveLib::Node* origin() const
    {
        return origin_.pointer();
    }
    /// User-defined pointer. In particular, used by formatter
    /// to keep pointer to FO chain.
    void*       userPtr() const { return userPtr_; }
    void        setUserPtr(void* p) { userPtr_ = p; }

    void        getTreeloc(COMMON_NS::TreelocRep& tl) const;

    void        notifyTemplateStateChange() const;

    virtual const GroveLib::Node* copyOrigin() const { return origin(); }

    PRTTI_DECL(XsltResult);

private:
    COMMON_NS::RefCntPtr<const GroveLib::Node> origin_;
    void*           userPtr_;
    const Xslt::Instance* instance_;
};

class XSLT_EXPIMP AttributeResult : public XsltResult,
                                    public GroveLib::Attr {
public:
    XSLT_OALLOC(AttributeResult);

    AttributeResult(const COMMON_NS::String& attrName,
                    const COMMON_NS::String& value,
                    const GroveLib::Node* origin,
                    const Xslt::Instance* i);
    virtual ~AttributeResult();

    GroveLib::Node* node() { return this; }
    PRTTI_DECL(XsltResult);

private:
    AttributeResult(const AttributeResult&);
    AttributeResult& operator=(const AttributeResult&);
};

class XSLT_EXPIMP ElementResult : public XsltResult,
                                  public GroveLib::Element {
public:
    XSLT_OALLOC(ElementResult);

    ElementResult(const COMMON_NS::String& name,
                  const GroveLib::Node* origin,
                  const Xslt::Instance* i);
    virtual ~ElementResult();

    GroveLib::Node* node() { return this; }
    PRTTI_DECL(XsltResult);

protected:
    GroveLib::Node* copy() const;

private:
    //! Usage disabled
    ElementResult(const ElementResult&);
    ElementResult& operator=(const ElementResult&);
};

class XSLT_EXPIMP TextResult : public XsltResult,
                               public GroveLib::Text {
public:
    XSLT_OALLOC(TextResult);
    TextResult(const COMMON_NS::String& text,
               const GroveLib::Node* origin,
               const Xslt::Instance* i);
    virtual ~TextResult();

    GroveLib::Node* node() { return this; }
    PRTTI_DECL(XsltResult);

protected:
    GroveLib::Node* copy() const;

private:
    TextResult(const TextResult&);
    TextResult&  operator=(const TextResult&);
};

} // namespace Xslt

#endif // XSLT_RESULT_H
