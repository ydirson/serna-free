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
#ifndef XSLT_TOP_STYLESHEET_INST_H
#define XSLT_TOP_STYLESHEET_INST_H

#include "grove/Grove.h"

#include "xpath/Expr.h"

#include "xslt/xslt_defs.h"
#include "xpath/NodeSet.h"
#include "xslt/impl/Instance.h"
#include "common/ModRegistry.h"

COMMON_NS_BEGIN
class MessageStream;
COMMON_NS_END

namespace Xslt {

class TopStylesheet;
class VariableInstance;
class Variable;
class SharedData;
class DocumentCache;
class ExternalFunction;
class FunctionRegistry;

typedef COMMON_NS::ModRegistry<Instance> ModificationRegistry;

class TopStylesheetInst : public Instance {
public:
    XSLT_OALLOC(TopStylesheetInst);
    typedef COMMON_NS::OwnerPtr<InstanceResult> InstanceResultOwner;

    TopStylesheetInst(const TopStylesheet* stylesheet,
                      Common::MessageStream* mstream,
                      Common::MessageStream* pstream,
                      const GroveLib::GrovePtr& source,
                      SharedData* sharedData,
                      FunctionRegistry* functionRegistry);
    virtual ~TopStylesheetInst();
    //!
    const GroveLib::GrovePtr&   resultGrove() const { return resultGrove_; }
    //!
    void                        update();
    //!
    virtual void                dump() const;
    //!
    Xpath::ValueHolder* getTopVar(const COMMON_NS::String& name) const;

    //! Reimplemented from Xpath::VarBindings
    Xpath::ValueHolder* getVariable(const COMMON_NS::String& name) const;

    DocumentCache*      documentCache() const { return docCache_.pointer(); }

    SharedData&         sharedData() const { return *sharedData_; }

    FunctionRegistry&   functionRegistry() const { return *functionRegistry_; }

    ModificationRegistry& modRegistry() const { return modRegistry_; }

    void                markModifiedNode(GroveLib::Node*);

    COMMON_NS::MessageStream* mstream() const { return mstream_; }
    COMMON_NS::MessageStream* pstream() const { return pstream_; }
    void setMessageStream(COMMON_NS::MessageStream* m) { mstream_ = m; }

private:
    class TopVarInstMap;
    class ModifiedNodesMap;

    Xpath::NodeSetItem          nsi_;
    GroveLib::GrovePtr          resultGrove_;
    InstanceResultOwner         instanceResult_;
    ResultContext               resultContext_;
    COMMON_NS::OwnerPtr<TopVarInstMap> topVarInstMap_;
    COMMON_NS::OwnerPtr<DocumentCache> docCache_;
    Common::RefCntPtr<SharedData> sharedData_;
    mutable ModificationRegistry modRegistry_;
    Common::OwnerPtr<ModifiedNodesMap> modifiedNodes_;
    FunctionRegistry*           functionRegistry_;
    COMMON_NS::MessageStream*   mstream_; //message stream for xsl:message
    COMMON_NS::MessageStream*   pstream_; 
};

} // namespace Xslt

#endif // XSLT_TOP_STYLESHEET_INST_H
