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

#ifndef _COMMON_URL_H_
#define _COMMON_URL_H_

#include "common/String.h"
#include "common/OwnerPtr.h"
#include "common/SernaApiBase.h"

namespace Common {

class COMMON_EXPIMP Url {
public:
    Url();
    Url(const Url&);
    Url& operator=(const Url&);

    /// Parse and init the URL
    Url(const String&);
    
    enum UrlProperty {
        PROTOCOL, USER, PASSWORD, HOST, 
        PORT, PATH, QUERY, DIRPATH, FILENAME, 
        COLLPATH, ENCODED_PATH_AND_QUERY, FRAGMENT
    };
    /// Query URL property
    String  operator[](const UrlProperty) const;
    
    /// Set URL property
    void    set(const UrlProperty, const String& value);
    
    /// Returns TRUE if URL is valid
    bool    isValid() const;
    
    /// Check if the URL is a local file
    bool    isLocal() const;

    /// Checks whether file exists (valid for local URL's only)
    bool    exists() const;
    
    /// Check if this is an relative filename (applies only to local URL)
    bool    isRelative() const;

    /// Returns absolute path name if isLocal(); returns same URL otherwise
    Url     absolute() const;   

    /// Combine current URL with the specified URL; note that both URL's 
    /// must not be dirnames
    Url     combinePath2Path(const Url& url) const;

    /// Combine current URL (which is assumed to be directory path)
    /// with the specified URL
    Url     combineDir2Path(const Url& url) const;

    /// Returns URL which is a relative-only suffix to the base URL
    Url     relativePath(const Url& url) const;

    /// Returns URL which points to upper path
    Url     upperUrl() const;
    
    /// Returns TRUE if both URL's point to the same location
    bool    operator==(const Url& other) const;
    bool    operator!=(const Url& other) const { return !operator==(other); }

    /// Conversion to String
    operator String() const; 
    
    static String encode(const String&);
    static String decode(const String&);
    
    ~Url();

private:
    class Impl;
    friend class OwnerPtr<Impl>;
    OwnerPtr<Impl> impl_;
};

} // namespace Common

#endif // _COMMON_URL_H_
