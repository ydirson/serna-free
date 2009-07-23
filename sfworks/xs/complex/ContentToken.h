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

#ifndef WILDCARD_TOKEN_H_
#define WILDCARD_TOKEN_H_

#include "xs/xs_defs.h"
#include "xs/complex/Wildcard.h"
#include "grove/XmlName.h"
#include "grove/Decls.h"
#include "common/RefCounted.h"
#include "common/RefCntPtr.h"

XS_NAMESPACE_BEGIN

class WildcardToken;
class QnameToken;
class SeqEndToken;
class Particle;

/*! Abstract interface to content tokens
 */
class ContentToken : public RefCounted<> {
public:
    enum tokenType {
        INPUT_TOKEN, QNAME_TOKEN, WILDCARD_TOKEN, SEQ_END_TOKEN
    };
    /*! Returns type of the token. There is three types of tokens:
     *  QNAME_TOKEN, which is matched by localName/URI pair;
     *  INPUT_TOKEN, which generated from input grove Element;
     *  and WILDCARD_TOKEN, which has URI wildcard.
     */
    tokenType       type() const { return type_; }

    /// Returns origin particle, if any
    Particle*       particle() const { return particle_; }

    virtual bool    match(const WildcardToken* t) const = 0;
    virtual bool    match(const QnameToken* t) const = 0;
    virtual bool    match(const ContentToken* t) const = 0;
    virtual bool    match(const SeqEndToken* t) const;

    // NOTE: less() and dataaddr() functions are used in NFA
    // and DFA builders to compare tokens by address of their content
    bool            less(const ContentToken* t) const;
    virtual const void* dataaddr() const;

    virtual String  format() const = 0;

    ContentToken(tokenType t, Particle* p)
#ifndef XS_DEBUG
        : particle_(p), type_(t) {}
#else // XS_DEBUG
        : particle_(p), type_(t), id_(++sequence_) {}
#endif // XS_DEBUG

    virtual ~ContentToken() {}

    XS_OALLOC(ContentToken);

protected:
    Particle*       particle_;
    tokenType       type_;
    COMMON_NS::String          format_proto(const char* toktype) const;

#ifdef XS_DEBUG
    uint    id_;
    static uint sequence_;
#endif // XS_DEBUG
};

/*! An end-of-sequence marker token
 */
class SeqEndToken : public ContentToken {
public:
    virtual bool    match(const WildcardToken* t) const;
    virtual bool    match(const QnameToken* t) const;
    virtual bool    match(const ContentToken* t) const;

    virtual COMMON_NS::String  format() const;

    SeqEndToken(Particle* p)
        : ContentToken(SEQ_END_TOKEN, p) {}
    virtual ~SeqEndToken() {}

    XS_OALLOC(SeqEndToken);
};

/*! WildcardToken - token in FSM which is a wildcard
 */
class WildcardToken : public ContentToken {
public:
    /*! Token wildcard
     */
    Wildcard*       wildcard() const;

    virtual bool    match(const WildcardToken* t) const;
    virtual bool    match(const QnameToken* t) const;
    virtual bool    match(const ContentToken* t) const;

    virtual COMMON_NS::String  format() const;

    WildcardToken(const Wildcard* w, Particle* p)
        : ContentToken(WILDCARD_TOKEN, p), wildcard_(w) {}

    virtual ~WildcardToken() {}

    XS_OALLOC(WildcardToken);

private:
    virtual const void* dataaddr() const;
    COMMON_NS::ConstRefCntPtr<Wildcard> wildcard_;
};

/*! QnameToken - token which has expanded qualified name (local+uri)
 */
class QnameToken : public ContentToken {
public:
    /*! Expanded name (localName + URI pair)
     */
    const GROVE_NAMESPACE::ExpandedName& ename() const { return ename_; }

    virtual bool    match(const WildcardToken* t) const;
    virtual bool    match(const QnameToken* t) const;
    virtual bool    match(const ContentToken* t) const;

    virtual String  format() const;

    QnameToken(const GROVE_NAMESPACE::ExpandedName& ename, Particle* p)
        : ContentToken(QNAME_TOKEN, p), ename_(ename) {}
    virtual ~QnameToken() {}

    XS_OALLOC(QnameToken);

protected:
    virtual const void* dataaddr() const;
    const GROVE_NAMESPACE::ExpandedName& ename_;
};

/*! A token which is generated from input elements
 */
class InputToken : public QnameToken {
public:
    /*! Origin element from which this token was generated
     */
    GROVE_NAMESPACE::Element* element() const { return elem_; }

    InputToken(GROVE_NAMESPACE::Element* elem);
    InputToken();

    virtual ~InputToken();

    XS_OALLOC(InputToken);

private:
    GROVE_NAMESPACE::ExpandedName expandedName_;
    GROVE_NAMESPACE::Element* elem_;
};

XS_NAMESPACE_END

#endif // WILDCARD_TOKEN_H_
