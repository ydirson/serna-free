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
#ifndef URI_RESOURCE_H_
#define URI_RESOURCE_H_

#include "urimgr/uri_defs.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"
#include "common/XTreePolicies.h"
#include "common/XTreeNode.h"
#include "common/String.h"

namespace CatMgr {
    class CatalogManager;
}

namespace Uri {

class ResourceBuilderList;

//
// Resources are organized into the tree, which reflects it's dependencies.
//
class URI_EXPIMP Resource : public COMMON_NS::RefCounted<>,
    public COMMON_NS::XTreeNode<Resource,
        COMMON_NS::XTreeNodeRefCounted<Resource> > {
public:
    typedef COMMON_NS::String String;

    enum ModtimeCheckState {
            SINGLE_CHECK, ALWAYS_CHECK, JUST_CHECKED, NEVER_CHECK
    };
    Resource(const String& type, const String& uri,
             ModtimeCheckState s = ALWAYS_CHECK);

    const String& type() const { return type_; }
    const String& uri() const  { return uri_; }

    /// Must return TRUE of resource was modified
    virtual bool isModified() const;
    void         setModified();

    /// Checks the current child list for the given resource;
    /// if not found, builds the resource and appends it as a child
    Resource* getResource(const String& resourceType,
                          const ResourceBuilderList& builderList,
                          const String& uri);

    /// Find resource within child list
    Resource* findResource(const String& resourceType,
                           const String& uri,
                           const CatMgr::CatalogManager* = 0);

    /// Release given resource (if found)
    bool      releaseResource(const String& resourceType,
                              const String& uri,
                              const CatMgr::CatalogManager* = 0);

    /// Obtain url by name relative to this resource
    String    getRelativeUrl(const String&) const;

    void      moveChildrenFrom(Resource* other);

    Resource* checkUpdate(const ResourceBuilderList& builderList);

    /// By default modtime of every resource is checked only once.
    /// This function should be called to enable modtime checks again.
    void      resetModtimeChecks(ModtimeCheckState s = ALWAYS_CHECK);
    ModtimeCheckState modtimeCheckState() const 
        { return (ModtimeCheckState)modtimeChecked_; }

    virtual void dump() const;

    virtual ~Resource() {}

private:
    String      type_;
    String      uri_;
    uint        forceModified_  : 1;
    uint        modtimeChecked_ : 3;
};

class URI_EXPIMP RootResource : public Resource {
public:
    RootResource();
    RootResource(const RootResource& other);
    virtual ~RootResource();

    Resource*   getTopResource(const String& resourceType,
                               const ResourceBuilderList& builderList,
                               const String& uri);
private:
    DEFAULT_ASSIGN_OP_DECL(RootResource)
};

typedef COMMON_NS::RefCntPtr<Resource> ResourcePtr;

URI_EXPIMP RootResource& uriManager();

URI_EXPIMP Common::String
map_uri(const CatMgr::CatalogManager*, const Common::String&);

class URI_EXPIMP ModtimeCheckGuard {
public:
    ModtimeCheckGuard(Resource::ModtimeCheckState s = Resource::SINGLE_CHECK,
            Resource::ModtimeCheckState after = Resource::ALWAYS_CHECK)
        : after_(after)
    {
        uriManager().resetModtimeChecks(s);
    }
    ~ModtimeCheckGuard()
    {
        uriManager().resetModtimeChecks(after_);
    }
private:
    Resource::ModtimeCheckState after_;
};

} // namespace Uri

#endif // URI_RESOURCE_H_
