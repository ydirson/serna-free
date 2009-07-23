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

#include "xslt/xslt_defs.h"
#include "xslt/XsltResource.h"
#include "xslt/ExternalFunction.h"
#include "xslt/impl/FunctionRegistry.h"
#include "xslt/impl/KeyGenerator.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/InstructionFactory.h"
#include "xslt/impl/FunctionRegistry.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/instances/SharedData.h"
#include "xslt/impl/instances/TopStylesheetInst.h"
#include "xslt/impl/EngineImpl.h"
#include "xslt/impl/WhitespaceStripper.h"
#include "xslt/XsltMessages.h"
#include "xslt/impl/DocumentHolder.h"
#include "xslt/TopParamValueHolder.h"

#include "urimgr/Resource.h"

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace Xslt {

Engine::~Engine() {}

EngineImpl::EngineImpl(Messenger* messenger,
                       MessageStream* pstream,
                       const CatMgr::CatalogManager* cat)
    : mstream_(XsltMessages::getFacility()), 
      pstream_(pstream)
{
    mstream_.setMessenger(messenger);
    xsltResourceBuilder_ = new XsltResourceBuilder(messenger, pstream, cat);
    functionRegistry_    = new FunctionRegistry;
}

void EngineImpl::setMessenger(COMMON_NS::Messenger* messenger)
{
    mstream_.setMessenger(messenger);
}

void EngineImpl::setStyle(const String& styleUri)
{
    if (styleUri.isEmpty())
        throw Xslt::Exception(XsltMessages::engineNoStyle);
    styleUri_ = styleUri;
    xsltResource_ = dynamic_cast<XsltResource*>(Uri::uriManager().
        getTopResource("xslt", *xsltResourceBuilder_, styleUri_));
    if (xsltResource_.isNull())
        throw Xslt::Exception(XsltMessages::engineNoStyleUri, styleUri);
    stylesheet_ = xsltResource_->topStyleSheet();
    stylesheet_->docHolder()->uriStack().push_back(styleUri);
}

const GrovePtr& EngineImpl::transform(const GrovePtr& sourceGrove,
                                      bool noStrip,
                                      Engine* sharedEngine)
{
    if (sourceGrove.isNull())
        throw Xslt::Exception(XsltMessages::engineNoSrc);
    if (stylesheet_.isNull())
        throw Xslt::Exception(XsltMessages::engineNoStyleDoc);
    if (!noStrip) {
        stylesheet_->whitespaceStripper()->
            stripTree(sourceGrove->document()->documentElement());
        Grove* g = sourceGrove->firstChild();
        for (; g; g = g->nextSibling())
            stylesheet_->whitespaceStripper()->
                stripTree(g->document()->documentElement());
    }    
    SharedData* sharedData = 0;
    if (sharedEngine)
        sharedData = &static_cast<EngineImpl*>(sharedEngine)->
            instance_->sharedData();
    try {
        instance_ = static_cast<TopStylesheetInst*>
            (stylesheet_->makeInst(&mstream_, pstream_, sourceGrove, 
                sharedData, functionRegistry_.pointer()));
    }
    catch (Xslt::Exception& e) {
        throw;
    }
    if (instance_.isNull())
        throw Xslt::Exception(XsltMessages::engineNoStyleInst);
    if (instance_->resultGrove().isNull())
        throw Xslt::Exception(XsltMessages::engineNoResult);
    return instance_->resultGrove();
}

void EngineImpl::update()
{
    if (!instance_.isNull())
        instance_->update();
}

String EngineImpl::getStyle() const
{
    return styleUri_;
}

TopStylesheet* EngineImpl::topStylesheet() const
{
    return stylesheet_.pointer();
}

TopStylesheetInst* EngineImpl::topStylesheetInst() const
{
    return instance_.pointer();
}

const GroveLib::StripInfo* EngineImpl::stripInfo() const
{
    return stylesheet_->whitespaceStripper();
}

TopParamValueHolder*
EngineImpl::getTopParamValueHolder(const Common::String& name) const
{
    return dynamic_cast<TopParamValueHolder*>
        (topStylesheetInst()->getTopVar(name));
}

void EngineImpl::getTopParamList(Common::PropertyNode* root) const
{
    stylesheet_->getTopParamList(root);
}

Engine* Engine::makeEngine(Messenger* messenger,
                           MessageStream* pstream,
                           const CatMgr::CatalogManager* catMgr)
{
    return new EngineImpl(messenger, pstream, catMgr);
}
    
void EngineImpl::registerExternalFunction(ExternalFunction* func)
{
    functionRegistry_->registerExternalFunction(func, true);
    func->xsltEngine_ = this;
}

void EngineImpl::deregisterExternalFunction(ExternalFunction* func)
{
    functionRegistry_->registerExternalFunction(func, false);
    func->xsltEngine_ = 0;
}

NumberCache* EngineImpl::numberCache() const
{
    return &instance_->sharedData().numberCache();
}

DocumentCache* EngineImpl::documentCache() const
{
    return instance_->documentCache();
}

ExternalFunction::~ExternalFunction()
{
    if (xsltEngine_)
        xsltEngine_->deregisterExternalFunction(this);
}

}
