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
#include "urimgr/Resource.h"
#include "urimgr/ResourceBuilder.h"
#include "urimgr/impl/uri_debug.h"
#include "catmgr/CatalogManager.h"
#include "common/Singleton.h"
#include "common/Url.h"

using namespace Common;

namespace Uri {

Resource::Resource(const String& type, 
                   const String& uri,
                   ModtimeCheckState s)
    : type_(type), uri_(uri), 
      forceModified_(false),
      modtimeChecked_(s)
{
}

bool Resource::isModified() const
{
    return forceModified_;
}

void Resource::setModified()
{
    forceModified_ = true;
}

void Resource::resetModtimeChecks(ModtimeCheckState s)
{
    modtimeChecked_ = s;
    for (Resource* r = firstChild(); r; r = r->nextSibling())
        r->resetModtimeChecks(s);
}

String Resource::getRelativeUrl(const String& path) const
{
    Url url(uri());
    return url.combinePath2Path(path);
}

Resource* Resource::findResource(const String& resourceType,
                                 const String& uri,
                                 const CatMgr::CatalogManager* catMgr)
{
    String mapped_uri(map_uri(catMgr, uri));
    DDBG << "findResource: type=" << resourceType << ", mapped="
        << mapped_uri << std::endl;
    Resource* rn = firstChild();
    for (; rn; rn = rn->nextSibling()) {
        if (rn->type_ == resourceType && rn->uri_ == mapped_uri)
            return rn;
    }
    return 0;
}

bool Resource::releaseResource(const String& resourceType,
                               const String& uri,
                               const CatMgr::CatalogManager* catMgr)
{
    Resource* rn = findResource(resourceType, uri, catMgr);
    if (0 == rn)
        return false;
    rn->remove();
    return true;
}

Resource* Resource::getResource(const String& resourceType,
                                const ResourceBuilderList& builderList,
                                const String& uri)
{
    DDBG << "getResource: src=<" << uri << ">\n";
    const ResourceBuilder* rb = builderList.getBuilder(resourceType);
    if (0 == rb)
        return 0;
    String mapped_uri(map_uri(builderList.catalogManager(), uri));
    Resource* rn = findResource(resourceType, mapped_uri);
    if (rn) {
        DDBG << "getResource: foundExisting=" << uri << std::endl;
        DBG_IF(URI.TEST) rn->dump();
        return rn->checkUpdate(builderList);
    }
    DDBG << "getResource: building, mapped=<" << mapped_uri << ">\n";
    return rb->build(this, mapped_uri);
}

Resource* Resource::checkUpdate(const ResourceBuilderList& builderList)
{
    if (modtimeCheckState() == JUST_CHECKED || 
        modtimeCheckState() == NEVER_CHECK)
            return this;
    if (modtimeCheckState() == SINGLE_CHECK)
        modtimeChecked_ = JUST_CHECKED;
    Resource* rn = firstChild();
    RefCntPtr<Resource> nextGuard;
        while (rn) {
                nextGuard = rn->nextSibling();
                (void) rn->checkUpdate(builderList);
                rn = nextGuard.pointer();
        }
    if (!isModified())
        return this;
    DDBG << "Rebuilding: " << type_ << ":" << uri_ << std::endl;
    parent()->setModified();
    const ResourceBuilder* rb = builderList.getBuilder(type_);
    if (0 == rb)
        return this;    // cannot rebuild
    RefCntPtr<Resource> new_res = rb->build(this, uri());
    if (!new_res.isNull()) {
        new_res->moveChildrenFrom(this);
        insertBefore(new_res.pointer());
    }
    remove();
    return new_res.pointer();
}

void Resource::moveChildrenFrom(Resource* other)
{
    RefCntPtr<Resource> rp;
    while (other->firstChild()) {
        rp = other->firstChild();
        rp->remove();
        appendChild(rp.pointer());
    }
}

void Resource::dump() const
{
#ifndef _NDEBUG
    DDBG << "#" << siblingIndex() << " " << type_
        << ": " << this << " [" << uri_ << "] MOD: " << isModified() << "\n";
    DDINDENT;
    for (const Resource* rn = firstChild(); rn; rn = rn->nextSibling())
        rn->dump();
#endif 
}

////////////////////////////////////////////////////////////////

RootResource::RootResource()
    : Resource("#ROOT-RESOURCE", "")
{
    incRefCnt();    // to avoid destruction via singleton
}

Resource* RootResource::getTopResource(const String& resourceType,
                                       const ResourceBuilderList& builderList,
                                       const String& uri)
{
    DDBG << "GetTopResource: " << uri << std::endl;
    RefCntPtr<Resource> result =
        getResource(resourceType, builderList, uri);
    if (!result.isNull() && !result->parent())
        appendChild(result.pointer());
    return result.pointer();
}

RootResource::RootResource(const RootResource& other)
 :  Resource(other)
{
}

RootResource::~RootResource()
{
}

RootResource& uriManager()
{
    return SingletonHolder<RootResource>::instance();
}

URI_EXPIMP Common::String
map_uri(const CatMgr::CatalogManager* catMgr, const Common::String& url)
{
    if (0 == catMgr)
        return url;
    String temp_url;
    temp_url = Url(url);
    String mapped_uri = catMgr->resolveExternalId(String(), temp_url);
    if (mapped_uri.isEmpty())
        mapped_uri = temp_url;
    temp_url = catMgr->resolveUri(mapped_uri);
    if (temp_url.isEmpty())
        temp_url = mapped_uri;
    return temp_url.isEmpty() ? url : temp_url;
}

} // namespace Uri

