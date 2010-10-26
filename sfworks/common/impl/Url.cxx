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
// Copyright (c) 2004 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

#include "common/Url.h"
#include "common/PathName.h"
#include "common/oalloc.h"
#include "common/StringTokenizer.h"
#include "common/impl/qurl_old.h"
#include <iostream>

namespace Common {

static bool is_urn(const Url& url)
{
    return url[Url::PROTOCOL] == "urn";
}
class Url::Impl : public QOldUrl {
public:
    USE_SUBALLOCATOR
    Impl() {}
    Impl(const String& s)
        : QOldUrl(s) 
    {
        if (protocol() == "urn")
            setPath(s);
    }
    Impl(const QOldUrl& url)
        : QOldUrl(url) {}
    Impl(const QOldUrl& url1, const String& url2, bool v)
        : QOldUrl(url1, url2, v) {}
};

bool Url::isValid() const
{
    return impl_->isValid();
}

bool Url::isLocal() const
{
    return !is_urn(*this) && impl_->isLocalFile();
}

bool Url::exists() const
{
    return !is_urn(*this) && PathName((*this)[PATH]).exists();
}
    
bool Url::isRelative() const
{
    return !is_urn(*this) && QOldUrl::isRelativeUrl((*this)[PATH]);
}

static bool is_same_base(const Url& url1, const Url& url2)
{
    if (is_urn(url1) || is_urn(url2))
        return false;
    if (url2[Url::PROTOCOL] == "file" && url2.isRelative())
        return true;
    return url1[Url::PROTOCOL] == url2[Url::PROTOCOL] &&
        url1[Url::HOST] == url2[Url::HOST] && 
        url1[Url::PORT] == url2[Url::PORT];
}

Url Url::absolute() const
{
    if (isLocal())
        return Url(PathName((*this)[PATH]).absolute().name());
    return *this;
}

Url Url::combinePath2Path(const Url& url) const
{
    if (!is_same_base(*this, url))
        return url;
    if (!url.isValid())
        return *this;
    Url result;
    if (isRelative() && url.isRelative()) {
        result = url;
        String my_path = impl_->dirPath();
        if (!my_path.isEmpty() && my_path != ".") {
            result.impl_->setPath(impl_->dirPath());
            result.impl_->addPath(url[PATH]);
        } 
        return result;
    }    
    result.impl_ = new Impl(*impl_, url, true);
    return result;
}

Url Url::combineDir2Path(const Url& url) const
{
    if (!is_same_base(*this, url))
        return url;
    if (!url.isValid())
        return *this;
    Url result;
    result.impl_ = new Impl(*impl_, url[PATH], false);
    return result;
}

Url Url::relativePath(const Url& url) const
{
    if (!is_same_base(*this, url))
        return url;
    if (!url.isValid())
        return *this;
    Url url_copy(url);
    String my_path  = impl_->dirPath();
    String his_path = url[PATH];
    if (isRelative() || url.isRelative() ) {
        if (my_path != ".") {
            url_copy.impl_->setPath(my_path);
            url_copy.impl_->addPath(his_path);
        } else
            url_copy.impl_->setPath(his_path);
        return url_copy;
    }
    my_path = (*this)[PATH];
    StringTokenizer my_tok(my_path, "/"), his_tok(his_path, "/");
    String my_last, his_last;
    uint pos = 0;
    while (my_tok && his_tok) {
        my_last = my_tok.next();
        his_last = his_tok.next();
        if (his_last != my_last)
            break;
        ++pos;
    }
    if (0 == pos)
        return url_copy;
    String result_path;
    if (!my_tok) {
        if (!his_tok)
            return Url(his_last);
        result_path = his_last;
        if (his_tok)
            result_path += '/';
        while (his_tok) {
            result_path += his_tok.next();
            if (his_tok)
                result_path += '/';
        }
        return Url(result_path);
    }
    if (!his_tok) {
        for (; my_tok; my_tok.next())
            result_path += NOTR("../");
        result_path += his_last;
        return Url(result_path);
    }
    for (; my_tok; my_tok.next()) 
        result_path += NOTR("../");
    result_path += his_last;
    if (his_tok)
        result_path += '/';
    while (his_tok) {
        result_path += his_tok.next();
        if (his_tok)
            result_path += '/';
    }
    return Url(result_path);
}

bool Url::operator==(const Url& other) const
{
    return impl_->QOldUrl::operator==(*other.impl_);
}

String Url::operator[](const UrlProperty prop) const
{
    switch (prop) {
        case PROTOCOL:
            return impl_->protocol();
        case USER:
            return impl_->user();
        case PASSWORD:
            return impl_->password();
        case HOST:
            return impl_->host();
        case PORT:
            return String::number(impl_->port());
        case PATH:
            return impl_->path();
        case QUERY:
            return impl_->query();
        case DIRPATH:
            return impl_->dirPath();
        case FILENAME:
            return impl_->fileName();
        case COLLPATH: {
            Url url_copy(*this);
            String s = url_copy[PATH];
            if (s.length() && s[s.length() - 1] != '/')
                url_copy.set(PATH, (*this)[DIRPATH] + "/");
            return url_copy.operator String();
        }
        case ENCODED_PATH_AND_QUERY:
            return impl_->encodedPathAndQuery();
        case FRAGMENT:
            return impl_->ref();
        default:
            return String();
    }
}

void Url::set(const UrlProperty prop, const String& value)
{
    switch (prop) {
        case PROTOCOL:
            impl_->setProtocol(value); return;
        case USER:
            impl_->setUser(value); return;
        case PASSWORD:
            impl_->setPassword(value); return;
        case HOST:
            impl_->setHost(value); return;
        case PORT:
            impl_->setPort(value.toUInt()); return;
        case PATH:
            impl_->setPath(value); return;
        case QUERY:
            impl_->setQuery(value); return;                        
        case DIRPATH: {
	    String dirpath = QOldUrl(value).dirPath();
            Impl url(dirpath);
            url.addPath(QOldUrl(value).fileName());
            *impl_ = url;
            return;
        }
        case FILENAME:
            impl_->setFileName(value); return;
        case ENCODED_PATH_AND_QUERY:
            impl_->setEncodedPathAndQuery(value);
            return;
        case FRAGMENT:
            impl_->setRef(value); return;
        default:
            return;
    }
}
    
Url Url::upperUrl() const
{
    String path((*this)[Url::PATH]);
    if (path.isEmpty())
        return Url();
    const Char* cstart = path.unicode();
    const Char* cp = cstart + path.length() - 1;
    const Char* fs = cstart;
    while (*fs != '/')
        ++fs;
    ++fs;
    if (*cp == '/')
        --cp;
    while (cp > fs && *cp != '/')
        --cp;
    if (cp <= cstart)
        return Url(); 
    Url upper_url(*this);
    upper_url.set(Url::PATH, String(cstart, cp - cstart));
    return upper_url;
}

String Url::encode(const String& s)
{
    QString qs = s;
    QOldUrl::encode(qs);
    return qs;
}

String Url::decode(const String& s)
{
    QString qs = s;
    QOldUrl::decode(qs);
    return qs;
}

Url::operator String() const
{
    if (is_urn(*this))
        return impl_->path();
    QString res = impl_->operator QString();
    if (res.startsWith("file:"))
        return res.mid(5);
    return res;
}

Url::Url()
    : impl_(new Impl)
{
}

Url::Url(const String& s)
    : impl_(new Impl(s))
{
}

Url::Url(const Url& other)
    : impl_(new Impl(*other.impl_))
{
}

Url& Url::operator=(const Url& other)
{
    impl_ = new Impl(*other.impl_);
    return *this;
}

Url::~Url()
{
}

} // namespace Common
