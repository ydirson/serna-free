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

#include "grove/grove_defs.h"
#include "grove/ExternalId.h"
#include "common/Singleton.h"

static const COMMON_NS::Char solidus('/');
static const COMMON_NS::Char minus('-');
static const COMMON_NS::Char plus('+');

GROVE_NAMESPACE_BEGIN

using namespace COMMON_NS;

const char *const PublicId::textClasses_[] = {
  "CAPACITY",   "CHARSET",  "DOCUMENT", "DTD",  "ELEMENTS",
  "ENTITIES",   "LPD",      "NONSGML",  "NOTATION", "SD",
  "SHORTREF",   "SUBDOC",   "SYNTAX",   "TEXT"
};

bool PublicId::parse(void)
{
    return parse(pubid_);
}

bool PublicId::parse(const String& pubid)
{
    valid_ = 0;
    if (pubid.isEmpty())
        return 0;
    if (pubid != pubid_)
        pubid_ = pubid;
    const Char* next = pubid.unicode();
    const Char* lim  = pubid.unicode() + pubid.length();
    const Char* fieldStart = 0;
    int fieldLength = 0;

    if (!nextField(solidus, next, lim, fieldStart, fieldLength)) {
        error_ = MissingField;
        return 0;
    }
    if (fieldLength == 1 && (*fieldStart == minus || *fieldStart == plus)) {
        ownerType_ = (*fieldStart == plus ? REGISTERED_PUBID : UNREGISTERED_PUBID);
        if (!nextField(solidus, next, lim, fieldStart, fieldLength)) {
            error_ = MissingField;
            return 0;
        }
    }
    else
        ownerType_ = ISO_PUBID;

    owner_.replace(0, fieldLength, fieldStart, fieldLength);

    if (!nextField(solidus, next, lim, fieldStart, fieldLength)) {
        error_ = MissingField;
        return 0;
    }
    int i = 0;
    for (; i < fieldLength; i++)
        if (fieldStart[i].isSpace())
            break;
    if (i >= fieldLength) {
        error_ = MissingTextClassSpace;
        return 0;
    }
    String textClassString(fieldStart, i);
    if (!lookup_textclass(textClassString, textClass_)) {
        error_ = InvalidTextClass;
        return 0;
    }
    i++;                          // skip the space
    fieldStart += i;
    fieldLength -= i;
    if (fieldLength  == 1 && *fieldStart == minus) {
        unavailable_ = 1;
        if (!nextField(solidus, next, lim, fieldStart, fieldLength)) {
            error_ = MissingField;
            return 0;
        }
    }
    else
        unavailable_ = 0;

    description_.replace(0, fieldLength, fieldStart, fieldLength);
    if (!nextField(solidus, next, lim, fieldStart, fieldLength)) {
        error_ = MissingField;
        return 0;
    }
    if (textClass_ != CHARSET) {
        for (i = 0; i < fieldLength; i++) {
            Char c = fieldStart[i];
            if (c < 'A' || c > 'Z') {
                error_ = InvalidLanguage;
                return 0;
            }
        }
        // The public text language must be a name.
        // Names cannot be empty.
        if (fieldLength == 0) {
            error_ = InvalidLanguage;
            return 0;
        }
    }
    lang_.replace(0, fieldLength, fieldStart, fieldLength);
    if (nextField(solidus, next, lim, fieldStart, fieldLength)) {
        switch (textClass_) {
            case CAPACITY:
            case CHARSET:
            case NOTATION:
            case SYNTAX:
                error_ = IllegalDisplayVersion;
                return 0;

            default:
              break;
        }
        hasDV_ = 1;
        displayVersion_.replace(0, fieldLength, fieldStart, fieldLength);
    }
    else
        hasDV_ = 0;

    if (next != 0) {
        error_ = ExtraField;
        return 0;
    }
    valid_ = 1;
    return 1;
}

bool PublicId::nextField(Char  solidus,
                         const Char*& next,
                         const Char*  lim,
                         const Char*& fieldStart,
                         int&  fieldLength)
{
    if (next == 0)
        return 0;
    fieldStart = next;
    for (; next < lim; next++) {
        if (next[0] == solidus && next + 1 < lim && next[1] == solidus) {
            fieldLength = next - fieldStart;
            next += 2;
            return 1;
        }
    }
    fieldLength = lim - fieldStart;
    next = 0;
    return 1;
}

bool PublicId::lookup_textclass(const String& s, TextClass& tc)
{
    for (ulong i = 0; i < (sizeof(textClasses_) / sizeof(textClasses_[0])); ++i) {
        if (String(textClasses_[i]) == s) {
            tc = TextClass(i);
            return 1;
        }
    }
    return 0;
}

// --- Documented public members ---

/*! Get owner type. Possible owner types are listed in OwnerType enum.
 */
PublicId::OwnerType PublicId::getOwnerType() const
{
    return ownerType_;
}

/*! Get owner as a string.
 */
const String& PublicId::getOwner() const
{
    return owner_;
}

/*! Get public ID text class. Possible text classes are listed
    in TextClass enum.
 */
PublicId::TextClass PublicId::getTextClass() const
{
    return textClass_;
}

/*! Check if public ID is "unavailable".
*/
bool PublicId::getUnavailable() const
{
    return unavailable_;
}

/*! Get public ID "description" as a string.
 */
const String& PublicId::getDescription() const
{
    return description_;
}

/*! Get public ID "language" field.
 */
const String& PublicId::getLanguage() const
{
    if (textClass_ == CHARSET)
        return String::null();
    return lang_;
}

/*! Get public ID designating sequence.
 */
const String& PublicId::getDesignatingSequence() const
{
    if (textClass_ != CHARSET)
        return String::null();
    return lang_;
}

/*! Get public ID display version
 */
const String& PublicId::getDisplayVersion() const
{
    if (hasDV_)
        return displayVersion_;
    return String::null();
}

GROVE_NAMESPACE_END
