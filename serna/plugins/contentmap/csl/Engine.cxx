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
#include "csl/Engine.h"
#include "csl/RootInstance.h"
#include "csl/Stylesheet.h"
#include "csl/Exception.h"
#include "csl/csl_debug.h"
#include "urimgr/GroveResource.h"
#include "urimgr/ResourceBuilder.h"
#include "common/Exception.h"
#include "utils/tr.h"
#include "grove/Nodes.h"

namespace Csl {

class Stylesheet;
class CslResourceBuilder;

// START_IGNORE_LITERALS
static const char CSL_RESOURCE_ID[] = "csl-stylesheet";
// STOP_IGNORE_LITERALS

class CslResource : public Uri::Resource {
public:
    Stylesheet*          stylesheet() const { return styleSheet_.pointer(); }
    void                 setStylesheet(Stylesheet* s) { styleSheet_ = s; }
    const CslResourceBuilder* builder() const { return builder_; }

    CslResource(const COMMON_NS::String& uri,
                Uri::GroveResource* grove,
                const CslResourceBuilder* xrb)
        : Resource(CSL_RESOURCE_ID, uri), 
          styleGrove_(grove), builder_(xrb) {}

private:
    Common::RefCntPtr<Stylesheet>         styleSheet_;
    Common::RefCntPtr<Uri::GroveResource> styleGrove_;
    const CslResourceBuilder*             builder_;
};

class CslResourceBuilderImpl : public Uri::ResourceBuilder {
public:
    CslResourceBuilderImpl()
        : ResourceBuilder(NOTR("csl-stylesheet")) {}
    virtual CslResource* build(Uri::Resource* parentResource,
                               const Common::String& uri) const;
};

class CslResourceBuilder : public Uri::ResourceBuilderList {
public:
    CslResourceBuilder(Common::Messenger* m,
                       const CatMgr::CatalogManager* cat)
        : ResourceBuilderList(cat)
    {
        appendChild(new Uri::SpGroveResourceBuilder(m));
        appendChild(new CslResourceBuilderImpl);
    }
};

class EngineImpl : public Engine {
public:
    virtual void        setStyle(const Common::String&);
    virtual void        setInstanceMaker(const InstanceMaker m)
    {
        instanceMaker_ = m;
    }
    virtual Instance*   transform(const GroveLib::Document*,
                                  const Common::String&);
    virtual void        update();

    virtual const Stylesheet* stylesheet() const
    { 
        return stylesheet_.pointer();
    }
    virtual const Instance* rootInstance() const 
    {
        return rootInstance_.pointer();
    }
    virtual void  setInstanceWatcher(InstanceWatcher* w)
    {
        if (!rootInstance_.isNull())
            rootInstance_->setInstanceWatcher(w);
    }
    virtual void  setNumberCache(Xslt::NumberCache* c) 
    {
        numberCache_ = c;
    }

    EngineImpl(Common::Messenger* m, const CatMgr::CatalogManager* cat) 
        : cslResourceBuilder_(m, cat),
          numberCache_(0),
          firstPass_(true)
    {
        DBG(CSL.LIFE) << "CSL: EngineImpl: " << this << std::endl;
    }
    ~EngineImpl()
    {
        DBG(CSL.LIFE) << "CSL: ~EngineImpl: " << this << std::endl;
    }

private:
    CslResourceBuilder              cslResourceBuilder_;
    InstanceMaker                   instanceMaker_;
    Common::RefCntPtr<Stylesheet>   stylesheet_;    
    Common::RefCntPtr<RootInstance> rootInstance_;
    Common::RefCntPtr<CslResource>  cslResource_;
    Xslt::NumberCache*              numberCache_;
    bool                            firstPass_;
};
    
CslResource* CslResourceBuilderImpl::build(Uri::Resource* parentRes,
                                           const Common::String& uri) const
{
    Common::RefCntPtr<Uri::GroveResource> gr =
        dynamic_cast<Uri::GroveResource*>(getResource(NOTR("grove"), parentRes, uri));
    if (gr.isNull())
        return 0;
    GroveLib::ElementPtr topNode = gr->grove()->document()->documentElement();
    if (topNode.isNull())
        return 0;
    Common::OwnerPtr<CslResource> xr(new CslResource(uri, gr.pointer(),
        static_cast<CslResourceBuilder*>(parent())));
    if (parentRes->type() == CSL_RESOURCE_ID) // rebuilding
        xr->moveChildrenFrom(parentRes);
    try {
        xr->setStylesheet(new Stylesheet(topNode.pointer()));
    } catch (Common::Exception& e) {
        throw CslException(e.what());
    }
    if (!gr->parent())
        xr->appendChild(gr.pointer());
    return xr.release();
}

///////////////////////////////////////////////////////////////////

void EngineImpl::setStyle(const Common::String& styleUri)
{
    if (styleUri.isEmpty())
        throw CslException(tr("Empty CSL URI: %0").arg(styleUri));
    cslResource_ = dynamic_cast<CslResource*>(Uri::uriManager().
        getTopResource(CSL_RESOURCE_ID, cslResourceBuilder_, styleUri));
    if (cslResource_.isNull())
        throw CslException(tr("Cannot parse CSL stylesheet: %0").arg(styleUri));
    stylesheet_ = cslResource_->stylesheet();
}

Instance* EngineImpl::transform(const GroveLib::Document* doc,
                                const Common::String& profileName)
{
    const Profile* profile = stylesheet_->findProfile(profileName);
    if (0 == profile)
        throw CslException(tr("No such profile: %0").arg(profileName));
    try {
        rootInstance_ = new RootInstance(
            const_cast<GroveLib::Document*>(doc), instanceMaker_, 
            stylesheet_.pointer(), profile, numberCache_, 
            firstPass_ ? RootInstance::FIRST_PASS : RootInstance::OPEN_PASS);
        firstPass_ = false;
    } 
    catch (Common::Exception& e) {
        throw CslException(e.what());
    }
    return rootInstance_.pointer();
}


void EngineImpl::update()
{
    try {   
        if (!rootInstance_.isNull())
            rootInstance_->update();
    } 
    catch(Common::Exception& e) {
        throw CslException(e.what());
    }
}

///////////////////////////////////////////////////////////////////

Engine* Engine::make(Common::Messenger* m,
                     const CatMgr::CatalogManager* cat)
{
    return new EngineImpl(m, cat);
}

} // namespace Csl
