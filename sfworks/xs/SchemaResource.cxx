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
#include "xs/xs_defs.h"
#include "xs/Schema.h"
#include "xs/SchemaResource.h"
#include "grove/Nodes.h"
#include "urimgr/GroveResource.h"
#include "common/MessageUtils.h"

using namespace Common;

namespace Xs {

class SchemaBuilderImpl : public Uri::ResourceBuilder {
public:
    SchemaBuilderImpl(Messenger* m, MessageStream* pstream)
        : ResourceBuilder("xml-schema"), messenger_(m), pstream_(pstream) {}

     virtual SchemaResource* build(Uri::Resource* parentResource,
                                   const String& uri) const;
private:
    RefCntPtr<Messenger>      messenger_;
    MessageStream*  pstream_;
};

SchemaResourceBuilder::SchemaResourceBuilder(Common::Messenger* m,
                                             Common::MessageStream* pstream,
                                             const CatMgr::CatalogManager* cat)
    : ResourceBuilderList(cat)
{
    appendChild(new Uri::SpGroveResourceBuilder(m));
    appendChild(new SchemaBuilderImpl(m, pstream));
}

SchemaResource* SchemaBuilderImpl::build(Uri::Resource* parentRes,
                                         const String& uri) const
{
    RefCntPtr<Uri::GroveResource> gr = dynamic_cast<Uri::GroveResource*>
        (getResource("grove", parentRes, uri));
    if (gr.isNull())
        return 0;
    if (!gr->grove()->document()->documentElement())
        return 0;
    OwnerPtr<SchemaResource> res(new SchemaResource
        (new Schema(&*messenger_), uri));
    if (pstream_) {
        res->schema()->setProgressStream(pstream_);
        String s = NOTR("XSV: loading ") + uri;
        *pstream_ << s;
    }
    res->schema()->setCatalogManager
        (static_cast<Uri::ResourceBuilderList*>(parent())->catalogManager());
    res->schema()->parse(gr->grove()->document()->documentElement());
    if (!gr->parent())
        res->appendChild(gr.pointer());
    return res.release();
}

SchemaResource::SchemaResource(Schema* s, const String& uri)
    : Resource("xml-schema", uri), schema_(s)
{
}

SchemaResource::~SchemaResource()
{
}

} // namespace Xs
