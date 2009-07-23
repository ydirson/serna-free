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

#ifndef EXTERNAL_ID_H
#define EXTERNAL_ID_H

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "common/String.h"

GROVE_NAMESPACE_BEGIN

/*! Parse public ID from character string and store it.
 */
class GROVE_EXPIMP PublicId {
public:
    enum TextClass {
        CAPACITY, CHARSET, DOCUMENT, DTD, ELEMENTS, ENTITIES,
        LPD, NONSGML, NOTATION, SD, SHORTREF, SUBDOC, SYNTAX, TEXT
    };

    enum OwnerType { ISO_PUBID, REGISTERED_PUBID, UNREGISTERED_PUBID };

    enum ErrorType {
        Unknown, MissingField, MissingTextClassSpace, MissingTextClass,
        InvalidTextClass, InvalidLanguage, IllegalDisplayVersion, ExtraField
    };

    /*! With default constructor, you probably should call parse(string) function later.*/
    PublicId();
    /*! Create and parse public ID from character string \a pubid.*/
    PublicId(const Common::String& pubid);

    /*! Returns public ID as a character string.*/
    const Common::String&   pubid(void) const;

    /*! Parse public ID from a character string \a pubid. Returns FALSE on errors.*/
    bool            parse(const Common::String& pubid);

    /*! (re)-parse already stored public ID. Returns FALSE on errors.*/
    bool            parse(void);

    /*! Check whether this public ID is valid (was parsed successfully).*/
    bool            valid(void) const;

    /*! Returns error code. Use only when parse() function returned FALSE.*/
    int       errcode(void) const;

    OwnerType       getOwnerType() const;
    const Common::String&   getOwner() const;
    TextClass                  getTextClass() const;
    const Common::String&   getDescription() const;
    const Common::String&   getLanguage() const;
    const Common::String&   getDesignatingSequence() const;
    bool                       getUnavailable() const;
    const Common::String&   getDisplayVersion() const;

    GROVE_OALLOC(PublicId);

private:
    bool            lookup_textclass(const Common::String&, TextClass&);
    bool            nextField(Common::Char, const Common::Char*&,
                              const Common::Char*,
                              const Common::Char*&, int&);
private:
    bool                       valid_;
    Common::String          pubid_;
    OwnerType                  ownerType_;
    Common::String          owner_;
    TextClass                  textClass_;
    Common::String          description_;
    Common::String          lang_;
    bool                       hasDV_;
    bool                       unavailable_;
    Common::String          displayVersion_;
    ErrorType                  error_;
    static const char* const textClasses_[];
};

/*! Representation of external ID (Public ID along with System ID)
 */
class GROVE_EXPIMP ExternalId {
public:
    const Common::String&   sysid(void) const;
    PublicId&       pubid(void);

    void            setSysid(const Common::String&);
    void            setPubid(const PublicId&);

    GROVE_OALLOC(ExternalId);

private:
    Common::String          sysid_;
    PublicId        pubid_;
};

/////////////////////////////////////////////////////////////////

inline PublicId::PublicId() : valid_(0), error_(Unknown) {}

inline PublicId::PublicId(const Common::String& pubid)
    : valid_(0), pubid_(pubid), error_(Unknown) { }

inline bool PublicId::valid(void) const
{
    return valid_;
}

inline const Common::String& PublicId::pubid(void) const
{
    return pubid_;
}

inline int PublicId::errcode(void) const
{
    return int(error_);
}

/////////////////////////////////////////////////////////////////

/*! Returns System ID string, if any
 */
inline const Common::String& ExternalId::sysid(void) const
{
    return sysid_;
}

/*! Returns reference to Public ID of this external ID. You may check
    validness of public ID using PublicId::valid() method.
 */
inline PublicId& ExternalId::pubid(void)
{
    return pubid_;
}

/*! Sets system ID string
 */
inline void ExternalId::setSysid(const Common::String& sysid)
{
    sysid_ = sysid;
}

/*! Sets public ID
 */
inline void ExternalId::setPubid(const PublicId& pubid)
{
    pubid_ = pubid;
}

GROVE_NAMESPACE_END

#endif // EXTERNAL_ID_H
