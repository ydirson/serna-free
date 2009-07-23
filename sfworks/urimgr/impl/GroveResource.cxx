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
#include "urimgr/GroveResource.h"
#include "urimgr/UrlResource.h"
#include "urimgr/impl/uri_debug.h"
#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "spgrovebuilder/SpGroveBuilder.h"
//#include "qtgrovebuilder/QtGroveBuilder.h"

using namespace Common;
using namespace GroveLib;

namespace Uri {

GroveResource::GroveResource(const String& url, GroveLib::Grove* g)
    : Resource("grove", url), grove_(g)
{
}

GroveResource::~GroveResource()
{
    DDBG << "GroveResource::~GroveResource\n";
}

void GroveResource::dump() const
{
    DDBG << "GroveResource: ";
    if (grove_) {
        DDBG << "TSYSID = " << grove_->topSysid() << std::endl;
    } else
        DDBG << "NULL_GROVE\n";
    Resource::dump();
}

/////////////////////////////////////////////////////////////////////

GroveResourceBuilder::GroveResourceBuilder(COMMON_NS::Messenger* m,
                                            Encodings::Encoding enc,
                                            GroveBuilder* gb)
    : ResourceBuilder("grove"), builder_(gb)
{
    if (m)
        builder_->setMessenger(m);
    if (enc != Encodings::XML)
        builder_->setEncoding(enc);
}

GroveResource* GroveResourceBuilder::build(Resource* parentResource,
                                           const String& uri) const
{
    DDBG << "GRB: build, URI = " << uri << "\n";
    UrlResource* fr =
        dynamic_cast<UrlResource*>(getResource("file", parentResource, uri));
    if (0 == fr)
        return 0;
    GroveBuilder* builder(builder_->copy());
    CatMgr::CatalogManager* catMgr = const_cast<CatMgr::CatalogManager*>
        (static_cast<ResourceBuilderList*>(parent())->catalogManager());
    if (catMgr)
        builder->setCatalogManager(catMgr);
    DDBG << "GRB: fr-url: " << fr->url() << std::endl;
    GrovePtr grove = builder->buildGroveFromFile(fr->url(), false);
    if (grove.isNull() || !grove->document()->documentElement())
        return false;
    DDBG << "GRB: return non-null grove\n";
    grove->setTopSysid(fr->url());
    GroveResource* gr = new GroveResource(fr->url(), grove.pointer());
    if (!fr->parent())
        gr->appendChild(fr);
    return gr;
}

SpGroveResourceBuilder::SpGroveResourceBuilder(COMMON_NS::Messenger* m,
                                               int flags,
                                               Encodings::Encoding enc)
    : GroveResourceBuilder(m, enc, new SpGroveBuilder(flags))
{
}

#if 0

QtGroveResourceBuilder::QtGroveResourceBuilder(COMMON_NS::Messenger* m,
                                               int flags,
                                               Encodings::Encoding enc)
    : GroveResourceBuilder(m, enc, new QtGroveBuilder(flags))
{
}

#endif

} // namespace Uri

