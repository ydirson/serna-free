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
#ifndef XSLT_IMPL_XSLT_RESOURCE_H_
#define XSLT_IMPL_XSLT_RESOURCE_H_

#include "xslt/xslt_defs.h"

#include "urimgr/GroveResource.h"
#include "urimgr/ResourceBuilder.h"

#include "common/Message.h"
#include "common/MessageUtils.h"
#include "common/RefCntPtr.h"
#include "common/OwnerPtr.h"

namespace Xslt {

class TopStylesheet;
class XsltResourceBuilder;

class XSLT_EXPIMP XsltResource : public Uri::Resource {
public:
    Xslt::TopStylesheet* topStyleSheet() const;
    void                 setTopStylesheet(Xslt::TopStylesheet* ts);

    const XsltResourceBuilder* builder() const { return xrb_; }
    Common::MessageStream* pstream() const;

    XsltResource(const COMMON_NS::String& uri,
                 Uri::GroveResource* grove,
                 const XsltResourceBuilder* xrb);

    virtual ~XsltResource();

private:
    COMMON_NS::RefCntPtr<Xslt::TopStylesheet>   styleSheet_;
    COMMON_NS::RefCntPtr<Uri::GroveResource>    styleGrove_;
    const XsltResourceBuilder*                  xrb_;
};

class XsltResourceBuilder : public Uri::ResourceBuilderList {
public:
    XsltResourceBuilder(COMMON_NS::Messenger* m,
                        COMMON_NS::MessageStream* progressStream,
                        const CatMgr::CatalogManager* = 0);
    virtual ~XsltResourceBuilder() {}

    Common::MessageStream* pstream() const { return pstream_; }

private:
    Common::MessageStream* pstream_;
};

} // namespace Xslt

#endif // XSLT_IMPL_XSLT_RESOURCE_H_
