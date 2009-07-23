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
/*! \file
 */

#ifndef NAMED_H
#define NAMED_H

#include "common/String.h"


GROVE_NAMESPACE_BEGIN

/*! Named object. Name is represented by String. */
class NamedBase {
public:
    NamedBase(const COMMON_NS::String &name) : name_(name) { }
    NamedBase() {}
    const COMMON_NS::String&   name() const { return name_; }
    const COMMON_NS::String*   namePointer() const { return &name_; }
    void            setName(const COMMON_NS::String &name) { name_ = name; }
    bool            operator<(const NamedBase& n){ return name_ < n.name(); }

private:
    COMMON_NS::String          name_;
};

class Named : public NamedBase {
public:
    Named(const COMMON_NS::String& name)
        : NamedBase(name) {
    }
    Named() {
    }
    virtual ~Named() {
    }
};

GROVE_NAMESPACE_END

#endif // NAMED_H
