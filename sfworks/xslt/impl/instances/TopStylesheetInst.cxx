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
#include "grove/Nodes.h"
#include "xpath/NodeSet.h"
#include "xpath/Engine.h"
#include "xslt/ExternalFunction.h"
#include "xslt/DocumentCache.h"
#include "xslt/impl/debug.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instructions/ApplicatingInstruction.h"
#include "xslt/impl/instructions/instructions.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "xslt/impl/instances/ApplicatingInstance.h"
#include "xslt/impl/instances/VariableInst.h"
#include "xslt/impl/instances/SharedData.h"
#include "common/PtrSet.h"
#include "grove/udata.h"
#include <set>
#include <map>

USING_COMMON_NS;

namespace Xslt {

class TopStylesheetInst::TopVarInstMap :
    public std::map<COMMON_NS::String, const VariableInstance*> {};

class TopStylesheetInst::ModifiedNodesMap : 
    public std::set<GroveLib::Node*> {};

TopStylesheetInst::TopStylesheetInst(const TopStylesheet* stylesheet,
                                     MessageStream* mstream,
                                     MessageStream* pstream,
                                     const GroveLib::GrovePtr& source,
                                     SharedData* sharedData,
                                     FunctionRegistry* functionRegistry)
    : Instance(stylesheet, InstanceInit(nsi_, resultContext_), 0),
      nsi_(source->document()),
      resultGrove_(new GroveLib::Grove),
      instanceResult_(new InstanceResult(0, resultGrove_->document())),
      resultContext_(resultGrove_->document(), 0),
      modifiedNodes_(new ModifiedNodesMap),
      functionRegistry_(functionRegistry),
      mstream_(mstream),
      pstream_(pstream)
{
    topVarInstMap_ = new TopVarInstMap;
    docCache_      = new DocumentCache(this);
    resultGrove_->setTopSysid(stylesheet->element()->grove()->topSysid());
    if (sharedData)
        sharedData_ = sharedData;
    else
        sharedData_ = new SharedData;
    COMMON_NS::RefCntPtr<ApplicatingInstance> appi =
        new ApplicatingInstance(stylesheet, stylesheet,
                InstanceInit(nsiContext(), resultContext_),
                this, false);
    appi->apply(resultContext_);
}

void TopStylesheetInst::update()
{
    Instance* instance = modRegistry().getFirst(true);
    for (; instance; instance = modRegistry().getFirst(true)) {
        DBG(XSLT.UPDATE) << "Updating: ";
        DBG_EXEC(XSLT.UPDATE, instance->dump());
        instance->update();
        instance->setModified(false);
    }
    if (!modifiedNodes_->empty()) {
        ModifiedNodesMap::iterator it = modifiedNodes_->begin();
        for (; it != modifiedNodes_->end(); ++it)
            GroveLib::set_node_modified(*it, false);
        modifiedNodes_->clear();
    }
}

void TopStylesheetInst::markModifiedNode(GroveLib::Node* n)
{
    modifiedNodes_->insert(n);
}

Xpath::ValueHolder*
TopStylesheetInst::getTopVar(const COMMON_NS::String& name) const
{
    DBG(XSLT.VARS) << "Entering GetTopVar, name " << name << std::endl;
    DBG(XSLT.VARS) << "TopVarInstMap size=" << topVarInstMap_->size()
        << std::endl;
    TopVarInstMap::const_iterator it = topVarInstMap_->find(name);
    if (it == topVarInstMap_->end()) {
        DBG(XSLT.VARS) << "Variable not found, instantiating" << std::endl;
        TopStylesheetInst* self = const_cast<TopStylesheetInst*>(this);
        const Variable* v =
            styleContext().topStylesheet()->getTopVariable(name);
        if (0 == v)
            return 0;
        const VariableInstance* vinst = 0;
        try {
            const Instance* vn = lastChild();
            v->makeInst(InstanceInit(nsiContext(), resultContext_), self);
            vn = vn ? vn->nextSibling() : firstChild();
            vinst = static_cast<const VariableInstance*>(vn);
            (*self->topVarInstMap_)[v->name()] = vinst;
            DBG(XSLT.VARS) << "Inserting variable " << v->name() << std::endl;
        }
        catch (Xslt::Exception& e) {
            throw Xslt::Exception(XsltMessages::instanceVar, e.what(),
                                  contextString());
        }
        DBG(XSLT.VARS) << "Returning inserted var OK" << std::endl;
        return vinst->getVariable();
    }
    DBG(XSLT.VARS) << "Returning existing var OK" << std::endl;
    return it->second->getVariable();
}

Xpath::ValueHolder*
TopStylesheetInst::getVariable(const COMMON_NS::String& name) const
{
    return getTopVar(name);
}

/////////////////////////////////////////////////////////////////

void TopStylesheetInst::dump() const
{
    DDBG << "TopStylesheetInst: " << this << std::endl;
    Instance::dump();
}

Instance* TopStylesheet::makeInst(MessageStream* ms,
                                  MessageStream* ps,
                                  const GroveLib::GrovePtr& source,
                                  SharedData* sharedData,
                                  FunctionRegistry* funcReg) const
{
    if (source.isNull() || (0 == source->document()->documentElement()))
        throw Xslt::Exception(XsltMessages::instrNoSrc, contextString());
    return new TopStylesheetInst(this, ms, ps, source, sharedData, funcReg);
}

TopStylesheetInst::~TopStylesheetInst()
{
    removeAllChildren();
}

} // namespace Xslt

