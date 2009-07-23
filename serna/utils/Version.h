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
// Copyright (c) 2003 Syntext Inc.
//
// This is a copyrighted commercial software.
// Please see COPYRIGHT file for details.

/** \file
 */

#ifndef UTILS_VERSION_H_
#define UTILS_VERSION_H_

#include "utils/utils_defs.h"
#include "common/StringDecl.h"

class UTILS_EXPIMP VersionNum {
public:
    typedef unsigned char NumType;
    VersionNum(NumType major = 0, NumType minor = 0, NumType patch = 0)
    {
        ver_[0] = major;
        ver_[1] = minor;
        ver_[2] = patch;
        ver_[3] = 0;
    }
    NumType getMajor() const { return ver_[0]; }
    NumType getMinor() const { return ver_[1]; }
    NumType getPatch() const { return ver_[2]; }
    bool    operator<(const VersionNum& rhs) const
        {
            return value() < rhs.value();
        }
    bool    operator==(const VersionNum& rhs) const
        {
            return value() == rhs.value();
        }

protected:
    unsigned    value() const
        {
            return (ver_[0] << 16 | ver_[1] << 8 | ver_[2]);
        }
private:
    NumType ver_[4];
};

class UTILS_EXPIMP Version {
public:
    //!
    typedef COMMON_NS::String String;
    //!
    virtual const String&       getStrVersion() const = 0;
    //!
    virtual const String&       buildDate() const = 0;
    //!
    virtual const VersionNum&   getNumVersion() const = 0;
    //! virtual constructor
    static const Version&       currentVersion();
    //!
    static const char*          version();
    //!
    static const char*          build_date();

protected:
    Version() {};
    virtual ~Version() {};

private:
    Version(const Version&);
    Version& operator=(const Version&);
};

#endif // UTILS_VERSION_H_
