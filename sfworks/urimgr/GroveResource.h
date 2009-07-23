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
#ifndef URI_GROVE_RESOURCE_H_
#define URI_GROVE_RESOURCE_H_

#include "urimgr/uri_defs.h"
#include "urimgr/Resource.h"
#include "urimgr/ResourceBuilder.h"
#include "grove/GroveBuilder.h"
#include "common/OwnerPtr.h"

namespace Uri {

class URI_EXPIMP GroveResource : public Resource {
public:
    GroveResource(const COMMON_NS::String& name, GroveLib::Grove* g);
    virtual ~GroveResource();

    GroveLib::Grove* grove() const { return grove_.pointer(); }

    virtual void    dump() const;

private:
    GroveLib::GrovePtr grove_;
};

///////////////////////////////////////////////////////////////////////

class URI_EXPIMP GroveResourceBuilder : public ResourceBuilder {
public:
    enum { buildFlags = GroveLib::GroveBuilder::pureXmlData |
        GroveLib::GroveBuilder::lineInfo };
    GroveResourceBuilder(COMMON_NS::Messenger* m,
                         Common::Encodings::Encoding enc,
                         GroveLib::GroveBuilder* gb);
    virtual GroveResource* build(Resource* parentResource,
                                 const COMMON_NS::String&) const;

private:
    COMMON_NS::OwnerPtr<GroveLib::GroveBuilder> builder_;
};

class URI_EXPIMP SpGroveResourceBuilder : public GroveResourceBuilder {
public:
    SpGroveResourceBuilder(COMMON_NS::Messenger* m = 0,
                           int flags = buildFlags,
                           Common::Encodings::Encoding enc =
                                Common::Encodings::XML);
};

#if 0

class URI_EXPIMP QtGroveResourceBuilder : public GroveResourceBuilder {
public:
    QtGroveResourceBuilder(COMMON_NS::Messenger* = 0,
                           int flags = buildFlags,
                           Common::Encodings::Encoding enc =
                                Common::Encodings::XML);
};

#endif

} // namespace Uri

#endif // URI_GROVE_RESOURCE_H_


