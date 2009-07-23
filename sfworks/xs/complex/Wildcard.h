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

#ifndef WILDCARD_H_
#define WILDCARD_H_

#include "xs/xs_defs.h"
#include "common/String.h"
#include "common/Vector.h"
#include "common/RefCounted.h"

class Schema;

XS_NAMESPACE_BEGIN
using COMMON_NS::String;
using COMMON_NS::Vector;
using COMMON_NS::RefCounted;
/*! A namespace wildcard. Wildcard is inherited from the Particle
    to ease FSM building from the wildcard. (???)
 */
class Wildcard : public RefCounted<> {
public:
    enum Type {
        ANY, OTHER, LIST
    };
    enum ProcessMode {
        NONE = 0, SKIP, LAX, WC_STRICT
    };
    Type            type() const { return type_; }
    ProcessMode     processMode() const { return pmode_; }

    virtual bool    match(const Wildcard* w) const = 0;
    virtual bool    match(const String& uri) const = 0;
    virtual String  format() const = 0;

    static Wildcard* make(Schema*,
                          ProcessMode pmode = WC_STRICT,
                          const String& = String());

    Wildcard(Type type, ProcessMode pmode)
        : type_(type), pmode_(pmode) {}

    virtual ~Wildcard() {}

private:
    Type        type_;
    ProcessMode pmode_;
};

class AnyNsWildcard : public Wildcard {
public:
    virtual bool    match(const Wildcard* w) const;
    virtual bool    match(const String& uri) const;
    virtual String  format() const;

    AnyNsWildcard(ProcessMode pmode)
        : Wildcard(Wildcard::ANY, pmode) {}

    virtual ~AnyNsWildcard() {}

    XS_OALLOC(AnyNsWildcard);
};

class OtherNsWildcard : public Wildcard {
public:
    virtual bool    match(const Wildcard* w) const;
    virtual bool    match(const String& uri) const;
    virtual String  format() const;

    OtherNsWildcard(const String& myns, ProcessMode pmode);

    virtual ~OtherNsWildcard() {}

    XS_OALLOC(OtherNsWildcard);

private:
    String  myns_;
    String  simplifiedMyns_;
};

class UriListWildcard : public Wildcard {
public:
    virtual bool    match(const Wildcard* w) const;
    virtual bool    match(const String& uri) const;
    virtual String  format() const;

    void            addUri(const String& uri);

    UriListWildcard(ProcessMode pmode)
        : Wildcard(Wildcard::LIST, pmode) {}

    virtual ~UriListWildcard() {}

    XS_OALLOC(UriListWildcard);

private:
    friend class OtherNsWildcard;
    COMMON_NS::Vector<String>  uriList_;
    COMMON_NS::Vector<String>  simplifiedUriList_;
};

XS_NAMESPACE_END

#endif // WILDCARD_H_
