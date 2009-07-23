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
#include "common/Url.h"
#include "common/safecast.h"
#include "sapi/common/Url.h"
#include <iostream>

namespace SernaApi {

#define SELF   ((Common::Url*) getData())

Url::Url(const SString& url)
{
    new (getData()) Common::Url(url);
}

Url::Url(const Common::Url& url)
{
    new (getData()) Common::Url(url);
}

Url::Url(const Url& url)
{
    new (getData()) Common::Url(url);
}

Url& Url::operator=(const Url& url)
{
    SELF->~Url();
    new (getData()) Common::Url(url);
    return *this;
}

Url::~Url()
{
    SELF->~Url();
}

Url::operator Common::Url() const
{
    return (const Common::Url&)(*getData());
}
    
SString Url::get(const UrlProperty property) const
{
    return SELF->operator[]((Common::Url::UrlProperty)(property));
}
    
void Url::set(const UrlProperty property, const SString& value)
{
    SELF->set((Common::Url::UrlProperty)(property), value);
}
    
bool Url::isValid() const
{
    return SELF->isValid();
}
    
    
bool Url::isLocal() const
{
    return SELF->isLocal();
}    

bool Url::exists() const
{
    return SELF->exists();
}  
    
bool Url::isRelative() const
{
    return SELF->isRelative();
}  

Url Url::absolute() const
{
    return SELF->absolute();
}  

Url Url::combinePath2Path(const Url& url) const
{
    return SELF->combinePath2Path(url);
}  

Url Url::combineDir2Path(const Url& url) const
{
    return SELF->combineDir2Path(url);
}  

Url Url::relativePath(const Url& url) const
{
    return SELF->relativePath(url);
}  

Url Url::upperUrl() const
{
    return SELF->upperUrl();
}  

bool Url::operator==(const Url& other) const
{
    return SELF->operator==(other);
}
    
bool Url::operator!=(const Url& other) const 
{ 
    return !operator==(other);
}
    
SString Url::asString() const
{
    return Common::String(*SELF);
}    
    
SString Url::encode(const SString& str)
{
    return Common::Url::encode(str);
}
    
SString Url::decode(const SString& str)
{
    return Common::Url::decode(str);
}

} // namespace SernaApi
