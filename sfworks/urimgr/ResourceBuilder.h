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
#ifndef URI_RESOURCE_BUILDER_H_
#define URI_RESOURCE_BUILDER_H_

#include "urimgr/uri_defs.h"
#include "common/String.h"
#include "common/XList.h"
#include "common/XTreePolicies.h"

namespace CatMgr {
    class CatalogManager;
}

namespace Uri {

class Resource;
class ResourceBuilderList;

class URI_EXPIMP ResourceBuilder : public COMMON_NS::RefCounted<>,
    public COMMON_NS::XListItem<ResourceBuilder,
        COMMON_NS::XTreeNodeRefCounted<ResourceBuilder> > {
public:
    ResourceBuilder(const COMMON_NS::String& type);

    const COMMON_NS::String& type() const { return type_; }

    /// Build the resource.
    virtual Resource* build(Resource* parentResource,
                            const COMMON_NS::String& uri) const = 0;

    /// Get builder for type, starting from this one
    const ResourceBuilder* getBuilder(const COMMON_NS::String& type) const;

    virtual ~ResourceBuilder();

protected:
    Resource*   getResource(const COMMON_NS::String& type,
                            Resource* parentResource,
                            const COMMON_NS::String& uri) const;
private:
    COMMON_NS::String type_;
};

class URI_EXPIMP ResourceBuilderList : public COMMON_NS::XList<ResourceBuilder,
    COMMON_NS::XTreeNodeRefCounted<ResourceBuilder> > {
public:
    ResourceBuilderList(const CatMgr::CatalogManager* = 0);
    const ResourceBuilder* getBuilder(const COMMON_NS::String& type) const;
    const CatMgr::CatalogManager* catalogManager() const { return catMgr_; }
    void setCatalogManager(const CatMgr::CatalogManager* cm) { catMgr_ = cm; }

private:
    const CatMgr::CatalogManager* catMgr_;
};

} // namespace Uri

#endif // URI_RESOURCE_BUILDER_H_


