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
#include "urimgr/UrlResource.h"
#include "urimgr/impl/uri_debug.h"
#include "dav/DavManager.h"
#include "common/PropertyTree.h"

using namespace Common;

namespace Uri {

static int get_mtime(const Url& url)
{
    DDBG << "UrlResource: getModtime: " << String(url) << std::endl;
    PropertyTree pt;
    if (Dav::DavManager::instance().getResourceInfo(url, pt.root()))
        return 0;
    return pt.root()->getSafeProperty("modtime")->getInt();
}

UrlResource::UrlResource(const Url& url)
    : Resource("file", url, uriManager().modtimeCheckState()),
      url_(url)
{                                     
    if (modtimeCheckState() != NEVER_CHECK)
        mtime_ = get_mtime(url_);
}

bool UrlResource::isModified() const
{
    if (modtimeCheckState() == NEVER_CHECK)
        return false;
    return get_mtime(url_) > (mtime_ + 1);
}

void UrlResource::dump() const
{
    DDBG << "UrlResource: " << String(url_) << " mtime " << mtime_ << "\n";
    Resource::dump();
}

UrlResource::~UrlResource()
{
    DDBG << "UrlResource::~UrlResource\n";
}

////////////////////////////////////////////////////////////////////

UrlResourceBuilder::UrlResourceBuilder()
    : ResourceBuilder("file")
{
}

UrlResource* UrlResourceBuilder::build(Resource* /*parentResource*/,
                                       const String& url) const
{
    DDBG << "UrlResource: build, URL = " << url << std::endl;
    ResourceBuilderList* rbl = static_cast<ResourceBuilderList*>(parent());
    String mapped_uri(map_uri(rbl->catalogManager(), url));
    return new UrlResource(mapped_uri);
}

} // namespace Uri
