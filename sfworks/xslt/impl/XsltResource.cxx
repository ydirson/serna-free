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
#include "xslt/xslt_defs.h"
#include "xslt/XsltResource.h"
#include "xslt/impl/Instruction.h"
#include "xslt/impl/InstructionFactory.h"
#include "xslt/impl/instructions/TopStylesheet.h"
#include "xslt/impl/WhitespaceStripper.h"
#include "common/Factory.h"
#include "urimgr/GroveResource.h"
#include "grove/GroveBuilder.h"
#include "grove/Nodes.h"
#include "grove/XNodePtr.h"
#include "common/ScopeGuard.h"
#include <iostream>

using namespace Common;
using namespace Xslt;

namespace Xslt {

TopStylesheet* XsltResource::topStyleSheet() const
{
    return styleSheet_.pointer();
}

void XsltResource::setTopStylesheet(TopStylesheet* ts)
{
    styleSheet_ = ts;
}

MessageStream* XsltResource::pstream() const
{
    return xrb_->pstream();
}

XsltResource::XsltResource(const String& uri,
                           Uri::GroveResource* grove,
                           const XsltResourceBuilder* xrb)
    : Resource("xslt", uri), styleGrove_(grove), xrb_(xrb)
{
}

XsltResource::~XsltResource()
{
}

////////////////////////////////////////////////////////////////

class XsltResourceBuilderImpl : public Uri::ResourceBuilder {
public:
    XsltResourceBuilderImpl(MessageStream* pstream)
        : ResourceBuilder("xslt"), pstream_(pstream) {}
    virtual XsltResource* build(Uri::Resource* parentResource,
                                const String& uri) const;
private:
    MessageStream*  pstream_;
};

XsltResourceBuilder::XsltResourceBuilder(Messenger* m,
                                         MessageStream* pstream,
                                         const CatMgr::CatalogManager* catMgr)
    : ResourceBuilderList(catMgr), pstream_(pstream)
{
    appendChild(new Uri::SpGroveResourceBuilder(m));
    appendChild(new XsltResourceBuilderImpl(pstream));
}

XsltResource* XsltResourceBuilderImpl::build(Uri::Resource* parentRes,
                                             const String& uri) const
{
    RefCntPtr<Uri::GroveResource> gr = dynamic_cast<Uri::GroveResource*>
        (getResource("grove", parentRes, uri));
    if (gr.isNull())
        return 0;
    GroveLib::ElementPtr topNode = gr->grove()->document()->documentElement();
    if (topNode.isNull())
        return 0;
    WhitespaceStripper ws;
    ws.stripStyle(topNode.pointer());
    InstructionFactory* factory =
        Common::Factory<InstructionFactory>::instance();
    factory->setProgressStream(pstream_);
    Instruction::Type type = factory->getTypeOf(topNode.pointer(), 0);
    if (Instruction::STYLESHEET != type)
        return 0;
    OwnerPtr<XsltResource> xr(new XsltResource(uri, gr.pointer(),
        static_cast<XsltResourceBuilder*>(parent())));
    if (parentRes->type() == "xslt") // rebuilding
        xr->moveChildrenFrom(parentRes);
    xr->setTopStylesheet(
        factory->makeTopStylesheet(topNode.pointer(), xr.pointer()));
    if (!gr->parent())
        xr->appendChild(gr.pointer());
    return xr.release();
}

} // namespace Xslt
