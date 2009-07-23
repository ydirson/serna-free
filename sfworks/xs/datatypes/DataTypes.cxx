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


#include "xs/datatypes/DataTypes.h"
#include "xs/datatypes/FacetsContainer.h"
#include "xs/datatypes/Facets.h"
#include "xs/datatypes/AtomicType.h"

#include "xs/EnumFetcher.h"
#include "xs/XsMessages.h"
#include "xs/Schema.h"
#include "common/String.h"
#include "common/StringCvt.h"                                                 
#include "common/StringCmp.h"
#include "xs/XsDataImpl.h"
#include "xs/complex/Particle.h"

#include "grove/Nodes.h"
#include "grove/Grove.h"
#include "grove/IdManager.h"
#include <math.h>
#include <qstringlist.h>
#include <limits.h>
#include <iostream>
#include <algorithm>

USING_COMMON_NS
USING_GROVE_NAMESPACE

namespace { //for NameType

bool check(const String& source, String& result)
{
    bool value_to_return = true;
    result = simplify_white_space(source);
    unsigned end = 0;
    unsigned start = 0;
    while (result.length() > ulong(start)) {
        end = result.find(' ', start);
        if (result.npos == end)
            end = result.length();
        String temp(mid(result, start, end - start));
        if (0 < temp.length()) {
            if ((starts_with(temp, "xml")) ||
                ((!temp[0].isLetter()) && ('_' != temp[0]) && (':' != temp[0])))
                value_to_return = false;
            //TODO: exact checking for Combining chars and Extenders.
        }
        else
            value_to_return = false;
        start = end + 1;
    }
    if (0 == result.length())
        value_to_return = false;

    return value_to_return;
};

//for IDType
bool check_id(Schema* schema, const GroveLib::Node* o,
              const String& source, String& result,
              bool isRef = false, bool isList = false)
{
    (void) isList;
    USING_XS_NAMESPACE
    GROVE_NAMESPACE::Grove* gr = 0;
    if (o)
        gr = o->grove();
    bool value_to_return = true;
    result = simplify_white_space(source);
    String temp(result);
    if (temp.contains(' ') || temp.contains(QChar(0xD))  ||
        temp.contains(QChar(0xA)) || temp.contains(QChar(0x9))) {
        if (0 != schema)
            schema->mstream() << XsMessages::notNCName << Message::L_ERROR
                              << temp << SRC_ORIGIN(o);
            value_to_return = false;
    }
    if (unsigned(temp.find(':')) != temp.npos) {
        if (0 != schema)
            schema->mstream() << XsMessages::notNCName << Message::L_ERROR
                              << temp << SRC_ORIGIN(o);
            value_to_return = false;
    }
    if (!temp.isNull()) {
        if ((!temp[0].isLetter()) &&  ('_' != temp[0])) {
            if (0 != schema)
                schema->mstream() << XsMessages::notName << Message::L_ERROR
                                  << temp << SRC_ORIGIN(o);
            return false;
        }
    }
    if (0 == gr)
        return value_to_return;
    GroveLib::IdManager* idm = gr->idManager();
    if (0 == idm)
        return value_to_return;
    GroveLib::Attr* a = 0;
    if (o->nodeType() == GroveLib::Node::ATTRIBUTE_NODE)
        a = static_cast<GroveLib::Attr*>(const_cast<GroveLib::Node*>(o));
    if (isRef) { // IDREF
        if (a && a->idClass() != GroveLib::Attr::IS_IDREF) {
            a->setIdClass(GroveLib::Attr::IS_IDREF);
            idm->attributeAdded(a);
            if (0 != schema)
                schema->idrefDelayedCheck(a);
        } else if (!idm->lookupElement(temp)) {
            if (0 != schema)
                schema->mstream() << XsMessages::notId << Message::L_ERROR
                                  << temp << SRC_ORIGIN(o);
            value_to_return = false;
        }
    } else if (a) { // ID
        if (a->idClass() != GroveLib::Attr::IS_ID) {
            a->setIdClass(GroveLib::Attr::IS_ID);
            idm->attributeAdded(a);
        }
        if (a->isDuplicateId()) {
            if (schema)
                schema->mstream() << XsMessages::duplicateId << Message::L_ERROR
                    << temp << SRC_ORIGIN(o);
            value_to_return = false;
        }
    }
    return value_to_return;
};

//for LanguageType
bool check_lang(const String& source, String& result)
{
    bool value_to_return = true;
    result = simplify_white_space(source);
    unsigned int end = 0;
    unsigned int start = 0;
    unsigned int pos = 0;
    while (result.length() > ulong(start)) {
        end = result.find(' ', start);
        if (result.npos == end)
            end = result.length();
        String temp(mid(result, start, end - start));
        pos = temp.find('-');
        if ((temp.npos != pos) && (pos < 3)) {
            if ((!starts_with(temp, "i-")) && (!starts_with(temp, "x-"))) {
                if ((!temp[0].isLetter()) && (!temp[1].isLetter()))
                    value_to_return = false;
            }
            if (value_to_return) {
                pos++;
                for (uint i = pos; i < temp.length(); i++) {
                    if ((!temp[pos].isLetter()) && (!temp[pos].isDigit())) {
                        value_to_return = false;
                        break;
                    }
                }
            }
        }
        else if (2 == temp.length()) {
          if ((!temp[0].isLetter()) && (!temp[1].isLetter())) {
              value_to_return = false;
          }
        }
        else {
            value_to_return = false;
        }
        start = end + 1;
    }
    return value_to_return;
};

//for NCNameType
bool check_nc(const String& source, String& result)
{
    bool value_to_return = true;
    result = simplify_white_space(source);
    unsigned int end = 0;
    unsigned int start = 0;
    while (result.length() > ulong(start)) {
        end = result.find(' ', start);
        if (result.npos == end)
            end = result.length();
        String temp(mid(result, start, end - start));
        if (unsigned(temp.find(':')) != temp.npos)
            value_to_return = false;
        start = end + 1;
    }

    return value_to_return;
};

//for NmtokenType
bool check_nm(const String& source, String& result)
{
    bool value_to_return = true;
    result = simplify_white_space(source);
    unsigned int end = 0;
    unsigned int start = 0;
    while (result.length() > ulong(start)) {
        end = result.find(' ', start);
        if (result.npos == end)
            end = result.length();
        String temp(mid(result, start, end - start));
        if (0 < temp.length()) {
            for (uint i = 0; i < temp.length(); i++) {
                if (temp[i].isLetter() || temp[i].isDigit() ||
                    '.' == temp[i] || '-' == temp[i] || '_' == temp[i] || ':' == temp[i])
                    continue;
                value_to_return = false;
            }
            //TODO: exact checking for Combining chars and Extenders.
        }
        else
           value_to_return = false;
        start = end + 1;
    }
    return value_to_return;
};

};

XS_NAMESPACE_BEGIN

/*****************************************************************************
 * RecurringType                                                             *
 *****************************************************************************/

RecurringType::RecurringType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred),
      startInstant_(0)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

RecurringType::~RecurringType()
{
    delete startInstant_;
}

bool RecurringType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::PERIOD:
            return setPeriod(schema, facet->asPeriodFacet());
        case XsFacet::DURATION:
            return setDuration(schema, facet->asDurationFacet());
        case XsFacet::MAXINCLUSIVE:
            return setMaxInclusive(schema, facet->asMaxInclusiveFacet());
        case XsFacet::MAXEXCLUSIVE:
            return setMaxExclusive(schema, facet->asMaxExclusiveFacet());
        case XsFacet::MININCLUSIVE:
            return setMinInclusive(schema, facet->asMinInclusiveFacet());
        case XsFacet::MINEXCLUSIVE:
            return setMinExclusive(schema, facet->asMinExclusiveFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet
                                  << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

void RecurringType::formLexicalRepresentation(String& result,
                                                      bool sign,
                                                      String& year,
                                                      String& month,
                                                      String& day,
                                                      String& hour,
                                                      String& minute,
                                                      String& sec,
                                                      String& msec) const
{
    result = year + '-' + month + '-' + day +
        'T' + hour + ':' + minute + ':' + sec + '.' + msec + 'Z';
    if (!sign)
        result = '-' + result;
}


bool checkDiapason(int start,int result,int period,int duration)
{
    if (0 != period) {
        int right = start +(((result - start) / period) * period);
        if (result == right)
            return true;
        if((result < right) ||
           (result >= right + duration))
            return false;
    }
    return true;
}

SchemaDateTime RecurringType::shift(const String& toShift, bool isIncrement) const
{
    SchemaDateTime val;
    toDateTime(val, toShift);

    if (AtomicType::YEAR == type()){
        SchemaDateTime temp(true, 1, 0, 0, 0, 0, 0);
        val = isIncrement ? val + temp : val - temp;
        return val;
    }

    if (AtomicType::YEARMONTH == type()){
        SchemaDateTime temp(true, 0, 1, 0, 0, 0, 0);
        val = isIncrement ? val + temp : val - temp;
        return val;
    }

    if ((AtomicType::DATE == type())          ||
        (AtomicType::MONTHDAY == type()) ||
        (AtomicType::DAY == type())) {
        SchemaDateTime temp(true, 0, 0, 1, 0, 0, 0);
        val = isIncrement ? val + temp : val - temp;
        return val;
    }

    if ((AtomicType::TIME == type())          ||
        (AtomicType::DATETIME == type())) {
        SchemaDateTime temp(true, 0, 0, 0, 0, 0, 1);
        val = isIncrement ? val + temp : val - temp;
        return val;
    }
    //TODO: think more about returning at this point.
    return val;
}

template<typename IntType> inline String
from_td(IntType v, unsigned width)
{
    return right_justify(from_integer<String>(v), width, '0', true);
}

bool
RecurringType::validate(Schema* schema, const GroveLib::Node* o, const String& source,
                        String& result) const
{
    bool value_to_return = true;
    result = simplify_white_space(source);
    SchemaDateTime val;
    if (!toDateTime(val, result)) {
        if (0 != schema)
            schema->mstream() << XsMessages::notRecurringDateTime
                              << Message::L_ERROR << result
                              << DV_ORIGIN(o);
        value_to_return = false;
        val = SchemaDateTime(true,1,0,0,0,0,0);
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ < val ) {
            result = maxInclFacet_->value();
            toDateTime(val, result);
            if (0 != schema)
                schema->mstream() << XsMessages::maxInclFacetConstrain
                                  << Message::L_ERROR << result
                                  << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= val ) {
            val = shift(maxExclFacet_->value(), false);
            //TODO:result = convert(val);
            result = maxExclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::maxExclFacetConstrain
                                  << Message::L_ERROR << maxExclFacet_->value()
                                  << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (val < minInclValue_) {
            result = minInclFacet_->value();
            toDateTime(val, result);
            if (0 != schema)
                schema->mstream() << XsMessages::minInclFacetConstrain
                                  << Message::L_ERROR << result
                                  << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != minExclFacet_.pointer()) {
        if (val <= minExclValue_) {
            val = shift(minExclFacet_->value(), true);
            //TODO:result = convert(val);
            result = minExclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::minExclFacetConstrain
                                  << Message::L_ERROR << minExclFacet_->value()
                                  << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);
    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); ++i) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain
                                  << Message::L_ERROR << result
                                  << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    if (0 == startInstant_) {
        //XS_NAMESPACE::XsDataImpl::Lock(*schema->xsi());
        startInstant_ = new SchemaDateTime(val);
    }
    else {
        if ((!periodFacet_.isNull()) && (!periodFacet_->value().isNull())) {
            bool is_valid = true;
            SchemaDateTime period = periodFacet_->value();
            SchemaDateTime duration = durationFacet_->value();
            SchemaDateTime target;
            toDateTime(target, result);
            is_valid = checkDiapason(startInstant_->year,target.year,
                                     period.year,duration.year);
            if (is_valid)
                is_valid = checkDiapason(startInstant_->month,target.month,
                                         period.month,duration.month);
            if (is_valid)
                is_valid = checkDiapason(startInstant_->day,target.day,
                                         period.day,duration.day);
            if (is_valid)
                is_valid = checkDiapason(startInstant_->hour,target.hour,
                                         period.hour,duration.hour);
            if (is_valid)
                is_valid = checkDiapason(startInstant_->minute,target.minute,
                                         period.minute,duration.minute);

            if ((is_valid) && (0 != period.second)) {
                double  right = startInstant_->second +
                    ((target.second - startInstant_->second) /
                     period.second) * period.second;
                if((target.second < right) ||
                   (target.second > right + durationFacet_->value().second))
                    is_valid = false;
                if (target.second == right)
                    is_valid = true;
            }
            if(!is_valid) {
                if (0 != schema)
                    schema->mstream() << XsMessages::recurringPeriodInvalid
                                      << Message::L_ERROR
                                      << result << DV_ORIGIN(o);
                value_to_return = false;
            }
            //TODO: build val
        }
    }

    String year(from_td(val.year, 4));
    String month(from_td(val.month, 2));
    String day(from_td(val.day, 2));

    String hour(from_td(val.hour, 2));
    String minute(from_td(val.minute, 2));
    String second(from_double<String>(val.second, 'g', 5));
    String sec, msec;
    unsigned pos = second.find('.');
    if (second.npos != pos) {
        sec = right_justify(second.substr(0, pos), 2, '0');
        msec = right_justify(second.substr(pos + 1), 3, '0');
    }
    else {
        sec = right_justify(second, 2, '0', true);
        msec.assign(3, '0');
    }

//    formLexicalRepresentation(result, val.sign, year, month, day, hour, minute, sec, msec);
    return value_to_return;
}

void RecurringType::dump(int) const
{
    std::cerr << NOTR("RecurringType") << std::endl;
}

AtomicType::Type RecurringType::type() const
{
    return AtomicType::RECURRING;
}

template<typename NumType> inline NumType
to_num(const String& s, unsigned pos, unsigned width, bool& ok)
{
    return to_integer<NumType, String>(mid(s, pos, width), &ok);
}

template<typename NumType> inline NumType
to_dbl(const String& s, unsigned pos, unsigned width, bool& ok)
{
    return to_double<NumType, String>(mid(s, pos, width), &ok);
}

bool RecurringType::toDateTime(SchemaDateTime& result, const String& str) const
{
    unsigned pos = 0;
    unsigned next = 0;

    if (!str[0].isDigit()) {
        if (AtomicType::MONTHDAY == type()) {
            if (starts_with(str, "---")) {
                result.sign = false;
                pos = pos + 2;
            }
            else if (starts_with(str, "--")) {
                pos++;
            }
            else
                return false;
        }
        else if (AtomicType::DAY == type()) {
            if (starts_with(str, "----")) {
                result.sign = false;
                pos = pos + 3;
            }
            else if (starts_with(str, "---")) {
                pos = pos + 2;
            }
            else
                return false;
        }
        else if('-' == str[0]) {
            result.sign = false;
        }
        else {
            return false;
        }
        pos++;
    }

    bool ok = true;
    if ((AtomicType::DATE == type())        ||
        (AtomicType::YEARMONTH == type())   ||
        (AtomicType::YEAR == type())        ||
        (AtomicType::RECURRING == type())   ||
        (AtomicType::DATETIME == type())) {
        next = pos + 4;
        result.year = to_num<int>(str, pos, next - pos, ok);
        if (!ok)
            return false;
    }
    else
        result.year = 0001;

    if ((AtomicType::DATE == type())      ||
        (AtomicType::YEARMONTH == type()) ||
        (AtomicType::MONTHDAY == type())  ||
        (AtomicType::RECURRING == type()) ||
        (AtomicType::DATETIME == type())) {
        if (AtomicType::MONTHDAY != type())
            pos = next + 1;
        next = pos + 2;
        result.month = to_num<int>(str, pos, next - pos, ok);
        if ((!ok) || ('-' != str[pos-1]) || (12 < result.month))
            return false;
    }
    else
        result.month = 01;

    if ((AtomicType::DATE == type())      ||
        (AtomicType::MONTHDAY == type())  ||
        (AtomicType::DAY == type())       ||
        (AtomicType::RECURRING == type()) ||
        (AtomicType::DATETIME == type())) {
        if (AtomicType::DAY != type())
            pos = next + 1;
        next = pos + 2;
        result.day = to_num<int>(str, pos, next - pos, ok);
        if ((!ok) || ('-' != str[pos-1]) || (31 < result.day))
            return false;
    }
    else
        result.day = 01;

    if ((AtomicType::TIME == type()) ||
        (AtomicType::DATETIME == type()) ||
        (AtomicType::RECURRING == type())) {
        if (AtomicType::TIME != type())
            pos = next + 1;
        next = pos + 2;
        result.hour = to_num<int>(str, pos, next - pos, ok);
        if (!ok)
            return false;
    }
    else
        result.hour = 00;

    if ((AtomicType::TIME == type()) ||
        (AtomicType::DATETIME == type()) ||
        (AtomicType::RECURRING == type())) {
        pos = next + 1;
        next = pos + 2;
        result.minute = to_num<int>(str, pos, next - pos, ok);
        if ((!ok) || (':' != str[pos-1]))
            return false;
    }
    else
        result.minute = 00;

    if ((AtomicType::TIME == type()) ||
        (AtomicType::DATETIME == type()) ||
        (AtomicType::RECURRING == type())) {
        pos = next + 1;
        Char pat[] = { '+', '-', 'Z' };
        next = str.find_first_of(pat, pos, sizeof(pat)/sizeof(pat[0]));
        if (str.npos == next)
            next = str.length();
        result.second = to_dbl<double>(str, pos, next - pos, ok);
        if ((!ok) || (':' != str[pos-1]))
            return false;
    }
    else
        result.second = 00.000;

    next--;

    Char signch = str[next];
    if ('+' == signch || '-' == signch) {
        bool ob;
        int hr_delta = to_num<int>(str, next + 1, 2, ok);
        int min_delta = to_num<int>(str, next + 4, 2, ob);
        if ((!ok) || (!ob))
            return false;
        int signv = '-' == signch ? -1 : 1;
        result.hour += signv * hr_delta;
        result.minute += signv * min_delta;
    }

    return true;
}

bool
RecurringType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, maxInclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notRecurringDateTime
                              << Message::L_ERROR
                              << maxInclFacet->value() << XS_ORIGIN;
        return false;
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet
                                  << Message::L_ERROR << NOTR("maxInclusive")
                                  << XS_ORIGIN;
            return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                              << NOTR("maxInclusive") << NOTR("maxExclusive")
                              << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (cur < val) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict
                                      << Message::L_ERROR << NOTR("maxInclusive")
                                      << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (cur < minInclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("maxInclusive")
                                  << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minExclFacet_.pointer()) {
        if (cur <= minExclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("maxInclusive")
                                  << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxInclFacet_ = maxInclFacet;
    maxInclValue_ = cur;
    return true;
}

bool
RecurringType::setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, maxExclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notRecurringDateTime
                              << Message::L_ERROR << maxExclFacet->value()
                              << XS_ORIGIN;
        return false;
    }

    if (0 != maxExclFacet_.pointer()){
        if (maxExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR
                                  << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != maxInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                              << NOTR("maxExclusive") << NOTR("maxInclusive")
                              << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for (i = enumeration_->begin(); i != enumeration_->end(); i++) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (cur <= val) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict
                                      << Message::L_ERROR << NOTR("maxExclusive")
                                      << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }


    if (0 != minInclFacet_.pointer()) {
        if (cur <= minInclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("maxExclusive")
                                  << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minExclFacet_.pointer()) {
        if (cur <= minExclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("maxExclusive")
                                  << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxExclFacet_ = maxExclFacet;
    maxExclValue_ = cur;
    return true;
}

bool
RecurringType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, minInclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notRecurringDateTime
                              << Message::L_ERROR << minInclFacet->value()
                              << XS_ORIGIN;
        return false;
    }

    if (0 != minInclFacet_.pointer()){
        if (minInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR
                                  << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                              << NOTR("minInclusive") << NOTR("minExclusive")
                              << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for (i = enumeration_->begin(); i != enumeration_->end(); ++i) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (val < cur) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict
                                      << Message::L_ERROR << NOTR("minInclusive")
                                      << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ < cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("minInclusive")
                                  << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("minInclusive")
                                  << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minInclFacet_ = minInclFacet;
    minInclValue_ = cur;
    return true;
}

bool
RecurringType::setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, minExclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notRecurringDateTime
                              << Message::L_ERROR << minExclFacet->value()
                              << XS_ORIGIN;
        return false;
    }

    if (0 != minExclFacet_.pointer()) {
        if (minExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR
                                  << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                              << NOTR("minExclusive") << NOTR("minInclusive")
                              << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for (i = enumeration_->begin(); i != enumeration_->end(); ++i) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (val <= cur) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict
                                      << Message::L_ERROR << NOTR("minExclusive")
                                      << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("minExclusive")
                                  << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("minExclusive")
                                  << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minExclFacet_ = minExclFacet;
    minExclValue_ = cur;
    return true;
}

bool RecurringType::setDuration(Schema* schema, DurationFacet* durationFacet)
{
    if (0 != durationFacet_.pointer()) {
        if (durationFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR
                                  << NOTR("duration") << XS_ORIGIN;
            return false;
        }
    }

    durationFacet_ = durationFacet;
    return true;
}

bool RecurringType::setPeriod(Schema* schema, PeriodFacet* periodFacet)
{
    if (0 != periodFacet_.pointer()) {
        if (periodFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR
                                  << NOTR("period") << XS_ORIGIN;
            return false;
        }
    }

    periodFacet_ = periodFacet;
    return true;
}

bool RecurringType::addPattern(Schema* schema, PatternFacet* pattern)
{
    //TODO:: check for valid pattern.
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool RecurringType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    //TODO: check with period and duration facet constrainings.
    SchemaDateTime cur;
    if (!toDateTime(cur, enumeration->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notRecurringDateTime
                              << Message::L_ERROR << enumeration->value()
                              << XS_ORIGIN;
        return false;
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ < cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("enumeration")
                                  << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (cur < minInclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (cur <= minExclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool RecurringType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

/*****************************************************************************
 * TimePeriodType                                                            *
 *****************************************************************************/

TimePeriodType::TimePeriodType(const Origin& origin,
                               const NcnCred& cred)
    : RecurringType(origin, cred)
{
    String zero = NOTR("P0Y");
    setPeriod(0, new PeriodFacet(zero, true));
}

void TimePeriodType::dump(int) const
{
    std::cerr << NOTR("TimePeriodType") << std::endl;
}

AtomicType::Type TimePeriodType::type() const
{
    return AtomicType::TIMEPERIOD;
}

/*****************************************************************************
 * DateTimeType                                                              *
 *****************************************************************************/

DateTimeType::DateTimeType(const Origin& origin, const NcnCred& cred)
    : RecurringType(origin, cred)
{
    String zero = NOTR("P0Y");
    setPeriod(0, new PeriodFacet(zero, true));
    setDuration(0, new DurationFacet(zero, true));
}

void DateTimeType::dump(int) const
{
    std::cerr << NOTR("DateTimeType") << std::endl;
}

AtomicType::Type DateTimeType::type() const
{
    return AtomicType::DATETIME;
}

CLONE_ATOMIC_TYPE(DateTimeType)

/*****************************************************************************
 * DateType                                                                  *
 *****************************************************************************/

DateType::DateType(const Origin& origin,
                   const NcnCred& cred)
    : TimePeriodType(origin, cred)
{
    String duration = "P1D";
    setDuration(0, new DurationFacet(duration, true));
}

void DateType::formLexicalRepresentation(String& result,
                                         bool sign,
                                         String& year,
                                         String& month,
                                         String& day,
                                         String& ,
                                         String& ,
                                         String& ,
                                         String& ) const
{
    result = year + '-' + month + '-' + day;
    if (!sign)
        result = '-' + result;
}

void DateType::dump(int) const
{
    std::cerr << NOTR("DateType") << std::endl;
}

AtomicType::Type DateType::type() const
{
    return AtomicType::DATE;
}

CLONE_ATOMIC_TYPE(DateType)

/*****************************************************************************
 * DayType                                                                   *
 *****************************************************************************/

DayType::DayType(const Origin& origin, const NcnCred& cred)
    : RecurringType(origin, cred)
{
    String period = NOTR("P1M");
    String duration = NOTR("P1D");
    setPeriod(0, new PeriodFacet(period, true));
    setDuration(0, new DurationFacet(duration, true));
}

void DayType::formLexicalRepresentation(String& result,
                                                 bool sign,
                                                 String&,
                                                 String&,
                                                 String& day,
                                                 String&,
                                                 String&,
                                                 String&,
                                                 String&) const
{
    result = NOTR("---") + day;
    if (!sign)
        result = '-' + result;
}

void DayType::dump(int) const
{
    std::cerr << NOTR("DayType") << std::endl;
}

AtomicType::Type DayType::type() const
{
    return AtomicType::DAY;
}

CLONE_ATOMIC_TYPE(DayType)

/*****************************************************************************
 * DoubleType                                                                *
 *****************************************************************************/

DoubleType::DoubleType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool DoubleType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::MAXINCLUSIVE:
            return setMaxInclusive(schema, facet->asMaxInclusiveFacet());
        case XsFacet::MAXEXCLUSIVE:
            return setMaxExclusive(schema, facet->asMaxExclusiveFacet());
        case XsFacet::MININCLUSIVE:
            return setMinInclusive(schema, facet->asMinInclusiveFacet());
        case XsFacet::MINEXCLUSIVE:
            return setMinExclusive(schema, facet->asMinExclusiveFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

bool DoubleType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool is_ok;
    bool value_to_return = true;
    double val = source.toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << source << DV_ORIGIN(o);
        value_to_return = false;
        result = "0";
    }
    else
        result = source.simplifyWhiteSpace();

    if (0 != maxInclFacet_.pointer()) {
        if (val > maxInclFacet_->value().toDouble()) {
            result = maxInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::maxInclFacetConstrain << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != maxExclFacet_.pointer()) {
        if (val >= maxExclFacet_->value().toDouble()) {
            double res;
            if (0 != minInclFacet_.pointer())
                res = (maxExclFacet_->value().toDouble() +
                       minInclFacet_->value().toDouble() ) / 2;
            else if (0 != minExclFacet_.pointer())
                res = (maxExclFacet_->value().toDouble() +
                       minExclFacet_->value().toDouble() ) / 2;
            else
                res = floor(maxExclFacet_->value().toDouble());
            result.setNum(res);
            if (0 != schema)
                schema->mstream() << XsMessages::maxExclFacetConstrain << Message::L_ERROR
                        << maxExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (val < minInclFacet_->value().toDouble()) {
            result = minInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::minInclFacetConstrain << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != minExclFacet_.pointer()) {
        if (val <= minExclFacet_->value().toDouble()) {
            double res;
            if (0 != maxInclFacet_.pointer())
                res = (minExclFacet_->value().toDouble() +
                       maxInclFacet_->value().toDouble() ) / 2;
            else if (0 != maxExclFacet_.pointer())
                res = (minExclFacet_->value().toDouble() +
                       maxExclFacet_->value().toDouble() ) / 2;
            else
                res = ceil(minExclFacet_->value().toDouble());
            result.setNum(res);
            if (0 != schema)
                schema->mstream() << XsMessages::minExclFacetConstrain << Message::L_ERROR
                        << minExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);
    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    return value_to_return;
}

void DoubleType::dump(int) const
{
    std::cerr << NOTR("DoubleType") << std::endl;
}

AtomicType::Type DoubleType::type() const
{
    return AtomicType::DOUBLE;
}

bool DoubleType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    bool is_ok;
    maxInclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << maxInclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != maxInclFacet_.pointer()){
        if(maxInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("maxInclusive") << NOTR("maxExclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (maxInclFacet->value().toDouble() < (*i)->value().toDouble() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("maxInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (maxInclFacet->value().toDouble() < minInclFacet_->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minExclFacet_.pointer()) {
        if (maxInclFacet->value().toDouble() <= minExclFacet_->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxInclFacet_ = maxInclFacet;
    return true;
}

bool DoubleType::setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet)
{
    bool is_ok;
    maxExclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << maxExclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != maxExclFacet_.pointer()) {
        if(maxExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("maxExclusive") << NOTR("maxInclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (maxExclFacet->value().toDouble() <= (*i)->value().toDouble() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("maxExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (maxExclFacet->value().toDouble() <= minInclFacet_->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (maxExclFacet->value().toDouble() <= minExclFacet_->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxExclFacet_ = maxExclFacet;
    return true;
}

bool DoubleType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    bool is_ok;
    minInclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << minInclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != minInclFacet_.pointer()){
        if(minInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("minInclusive") << NOTR("minExclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ((*i)->value().toDouble() < minInclFacet->value().toDouble() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("minInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (maxInclFacet_->value().toDouble() < minInclFacet->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclFacet_->value().toDouble() <= minInclFacet->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minInclFacet_ = minInclFacet;
    return true;
}

bool DoubleType::setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet)
{
    bool is_ok;
    minExclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << minExclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != minExclFacet_.pointer()) {
        if(minExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("minExclusive") << NOTR("minInclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ((*i)->value().toDouble() <= minExclFacet->value().toDouble() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("minExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (maxInclFacet_->value().toDouble() <= minExclFacet->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclFacet_->value().toDouble() <= minExclFacet->value().toDouble() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minExclFacet_ = minExclFacet;
    return true;
}


bool DoubleType::addPattern(Schema* schema, PatternFacet* pattern)
{
    //TODO:: check for valid pattern.
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool DoubleType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    bool is_ok;
    double val = test.toDouble(&is_ok);
    if (!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << test << XS_ORIGIN;
        return false;
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclFacet_->value().toDouble() < val) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclFacet_->value().toDouble() <= val) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (val < minInclFacet_->value().toDouble()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (val <= minExclFacet_->value().toDouble()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool DoubleType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(DoubleType)

/*****************************************************************************
 * DurationType                                                              *
 *****************************************************************************/


DurationType::DurationType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool DurationType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::MAXINCLUSIVE:
            return setMaxInclusive(schema, facet->asMaxInclusiveFacet());
        case XsFacet::MAXEXCLUSIVE:
            return setMaxExclusive(schema, facet->asMaxExclusiveFacet());
        case XsFacet::MININCLUSIVE:
            return setMinInclusive(schema, facet->asMinInclusiveFacet());
        case XsFacet::MINEXCLUSIVE:
            return setMinExclusive(schema, facet->asMinExclusiveFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

bool DurationType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool value_to_return = true;
    result = source.simplifyWhiteSpace();
    SchemaDateTime val;
    if (!toDateTime(val, result)) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDateTime << Message::L_ERROR << result << DV_ORIGIN(o);
        value_to_return = false;
        val = SchemaDateTime();
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ < val ) {
            result = maxInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::maxInclFacetConstrain << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= val ) {
            //TODO: decrement on atom
            result = maxExclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::maxExclFacetConstrain << Message::L_ERROR
                        << maxExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (val < minInclValue_) {
            result = minInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::minInclFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != minExclFacet_.pointer()) {
        if (val <= minExclValue_) {
            //TODO: increment on atom
            result = minExclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::minExclFacetConstrain << Message::L_ERROR
                        << minExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);
    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    return value_to_return;
}

void DurationType::dump(int) const
{
    std::cerr << NOTR("DurationType") << std::endl;
}

AtomicType::Type DurationType::type() const
{
    return AtomicType::DURATION;
}

bool DurationType::toDateTime(SchemaDateTime& result, const String& str)
{
    int pos = 0;
    int next = 0;

    if (!str.length())
        return false;

    if (('P' != str[0]) && ('T' != str[0])){
        if('-' == str[0]) {
            result.sign = false;
            pos++;
        }
        else if('+' == str[0]) {
            pos++;
        }
        else
            return false;
    }

    if (pos >= 0 && uint(pos) < str.length() && 'P' == str[pos]) {
        pos++;

        next = str.find('Y', pos);
        if (0 <= next) {
            bool ok;
            result.year = (str.mid(pos, next-pos)).toInt(&ok);
            if (!ok) {
                result.year = 0;
            }
            pos = next + 1;
        }

        next = str.find('M', pos);
        if (0 <= next) {
            bool ok;
            result.month = (str.mid(pos, next-pos)).toInt(&ok);
            if (!ok) {
                result.month = 0;
            }
            pos = next + 1;
        }

        next = str.find('D', pos);
        if (0 <= next) {
            bool ok;
            result.day = (str.mid(pos, next-pos)).toInt(&ok);
            if (!ok) {
                result.day = 0;
            }
            pos = next + 1;
        }
    }

    if (pos >= 0 && uint(pos) < str.length() && 'T' == str[pos]) {
        pos++;
        next = str.find('H', pos);
        if (0 <= next) {
            bool ok;
            result.hour = (str.mid(pos, next-pos)).toInt(&ok);
            if (!ok) {
                result.hour = 0;
            }
            pos = next + 1;
        }

        next = str.find('M', pos);
        if (0 <= next) {
            bool ok;
            result.minute = (str.mid(pos, next-pos)).toInt(&ok);
            if (!ok) {
                result.minute = 0;
            }
            pos = next + 1;
        }

        next = str.find('S', pos);
        if(0 <= next) {
            bool ok;
            result.second = (str.mid(pos, next-pos)).toDouble(&ok);
            if (!ok) {
                result.second = 0;
            }
            pos = next + 1;
        }
    }
    return true;
}


bool DurationType::setMaxInclusive(Schema* schema,
                                       MaxInclusiveFacet* maxInclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, maxInclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDateTime << Message::L_ERROR
                    << maxInclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != maxInclFacet_.pointer()){
        if(maxInclFacet_->fixed()){
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("maxInclusive") << NOTR("maxExclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (cur < val) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("maxInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (cur < minInclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (cur <= minExclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxInclFacet_ = maxInclFacet;
    maxInclValue_ = cur;
    return true;
}

bool DurationType::setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, maxExclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDateTime << Message::L_ERROR
                    << maxExclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != maxExclFacet_.pointer()){
        if(maxExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("maxExclusive") << NOTR("maxInclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (cur <= val) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("maxExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }


    if(0 != minInclFacet_.pointer()) {
        if (cur <= minInclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (cur <= minExclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxExclFacet_ = maxExclFacet;
    maxExclValue_ = cur;
    return true;
}

bool DurationType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, minInclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDateTime << Message::L_ERROR
                    << minInclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != minInclFacet_.pointer()){
        if(minInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("minInclusive") << NOTR("minExclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (val < cur) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("minInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ < cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minInclFacet_ = minInclFacet;
    minInclValue_ = cur;
    return true;
}

bool DurationType::setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, minExclFacet->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDateTime << Message::L_ERROR
                    << minExclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != minExclFacet_.pointer()) {
        if(minExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("minExclusive") << NOTR("minInclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            SchemaDateTime val;
            toDateTime(val, (*i)->value());
            if (val <= cur) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("minExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minExclFacet_ = minExclFacet;
    minExclValue_ = cur;
    return true;
}


bool DurationType::addPattern(Schema* schema, PatternFacet* pattern)
{
    //TODO:: check for valid pattern.
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool DurationType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    SchemaDateTime cur;
    if (!toDateTime(cur, enumeration->value())) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDateTime << Message::L_ERROR << enumeration->value() << XS_ORIGIN;
        return false;
    }

    if(0 != maxInclFacet_.pointer()) {
        if (maxInclValue_ < cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclValue_ <= cur) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (cur < minInclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (cur <= minExclValue_) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool DurationType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(DurationType)

/*****************************************************************************
 * IntegerType                                                               *
 *****************************************************************************/

IntegerType::IntegerType(const Origin& origin, const NcnCred& cred)
    : NumberType(origin, cred)
{
    setScale(0, new FractionDigitsFacet(0, true));
}

bool IntegerType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    return NumberType::validate(schema, o, source, result);
}


void IntegerType::dump(int) const
{
    std::cerr << NOTR("IntegerType") << std::endl;
}

AtomicType::Type IntegerType::type() const
{
    return AtomicType::INTEGER;
}

CLONE_ATOMIC_TYPE(IntegerType)

/*****************************************************************************
 * MonthDayType                                                              *
 *****************************************************************************/


MonthDayType::MonthDayType(const Origin& origin, const NcnCred& cred)
    : RecurringType(origin, cred)
{
    String period = NOTR("P1Y");
    String duration = NOTR("P1D");
    setPeriod(0, new PeriodFacet(period, true));
    setDuration(0, new DurationFacet(duration, true));
}

void MonthDayType::formLexicalRepresentation(String& result,
                                                  bool sign,
                                                  String&,
                                                  String& month,
                                                  String& day,
                                                  String&,
                                                  String&,
                                                  String&,
                                                  String&) const
{
    result = NOTR("--") + month + '-' + day;
    if (!sign)
        result = '-' + result;
}

void MonthDayType::dump(int) const
{
    std::cerr << NOTR("MonthDayType") << std::endl;
}

AtomicType::Type MonthDayType::type() const
{
    return AtomicType::MONTHDAY;
}

CLONE_ATOMIC_TYPE(MonthDayType)

/*****************************************************************************
 * MonthType                                                                 *
 *****************************************************************************/

MonthType::MonthType(const Origin& origin, const NcnCred& cred)
    : TimePeriodType(origin, cred)
{
    String duration = NOTR("P1M");
    setDuration(0, new DurationFacet(duration, true));
}

void MonthType::formLexicalRepresentation(String& result,
                                          bool sign,
                                          String& year,
                                          String& month,
                                          String& ,
                                          String& ,
                                          String& ,
                                          String& ,
                                          String& ) const
{
    result = year + '-' + month;
    if (!sign)
        result = '-' + result;
}

void MonthType::dump(int) const
{
    std::cerr << NOTR("MonthType") << std::endl;
}

AtomicType::Type MonthType::type() const
{
    return AtomicType::MONTH;
}

CLONE_ATOMIC_TYPE(MonthType)

/*****************************************************************************
 * NameType                                                                  *
 *****************************************************************************/

NameType::NameType(const Origin& origin, const NcnCred& cred)
    : TokenType(origin, cred)
{
}

bool NameType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool value_to_return = check(source, result);
    if (value_to_return) {
        return TokenType::validate(schema, o, source, result);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notName << Message::L_ERROR << source << DV_ORIGIN(o);
        String s = NOTR("name");
        TokenType::validate(schema, o, s, result);
        return false;
    }
}

bool NameType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test;
    bool is_ok = check(enumeration->value(), test);
    if (is_ok) {
        return TokenType::addEnumeration(schema, enumeration);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notName << Message::L_ERROR << enumeration->value() << XS_ORIGIN;
        return false;
    }
}

void NameType::dump(int) const
{
    std::cerr << NOTR("NameType") << std::endl;
}

AtomicType::Type NameType::type() const
{
    return AtomicType::NAME;
}

CLONE_ATOMIC_TYPE(NameType)

/*****************************************************************************
 * NumberType                                                                *
 *****************************************************************************/

NumberType::NumberType(  const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool NumberType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::PRECISION:
            return setPrecision(schema, facet->asTotalDigitsFacet());
        case XsFacet::SCALE:
            return setScale(schema, facet->asFractionDigitsFacet());
        case XsFacet::MAXINCLUSIVE:
            return setMaxInclusive(schema, facet->asMaxInclusiveFacet());
        case XsFacet::MAXEXCLUSIVE:
            return setMaxExclusive(schema, facet->asMaxExclusiveFacet());
        case XsFacet::MININCLUSIVE:
            return setMinInclusive(schema, facet->asMinInclusiveFacet());
        case XsFacet::MINEXCLUSIVE:
            return setMinExclusive(schema, facet->asMinExclusiveFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

bool NumberType::checkScale(bool isToChange, String& result) const
{
    bool value_to_return = true;
    if ( 0 != scale_.pointer()) {
        if (0 < scale_->value()) {
            int decimal_pos = result.find('.');
            if ((result.length() != scale_->value() + decimal_pos + 1) ||
                (0 > decimal_pos)) {
                if (isToChange) {
                    if(0 > decimal_pos) {
                        result = result + '.';
                        decimal_pos = result.length() - 1;
                    }
                    result = result.leftJustify(scale_->value() + decimal_pos + 1,'0', true);
                }
                value_to_return = false;
            }
        }
        else if (0 == scale_->value()) {
            int decimal_pos = result.find('.');
            if(0 <= decimal_pos) {
                if (isToChange) {
                    result = result.leftJustify(decimal_pos,'0', true);
                }
                value_to_return = false;
            }
        }
    }
    return value_to_return;
}


bool NumberType::checkPrecision(bool isToChange, String& result) const
{
    bool value_to_return = true;
    if (0 != precision_.pointer()) {
        bool is_negative = false;
        if (result[0] == '-')
            is_negative = true;

        if (0 < precision_->value()) {
            uint len = precision_->value();
            if (result.npos != unsigned(result.find('.')))
                len++;
            if (len >= result.length()) {
                len = result.length();
            }
            else {
                if (isToChange) {
                    result = right(result, len);
                    if ((result[0].isDigit()) && (is_negative))
                        result.insert(0, '-');
                }
                value_to_return = false;
            }
        }
    }
    return value_to_return;
}


void NumberType::shift(const String& toShift, bool isIncrement, String& result) const
{
    int decimals = scale_->value();

    double diff;
    if (0 == decimals) {
        diff = 1;
    }
    else {
        String temp = NOTR("0.");
        temp = temp.leftJustify(decimals+1, '0');
        temp = temp + '1';
        diff = temp.toDouble();
    }

    if (isIncrement) {
        result.setNum(toShift.toDouble() + diff);
    }
    else {
        result.setNum(toShift.toDouble() - diff);
    }
}


bool NumberType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool is_ok;
    bool value_to_return = true;
    source.toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << source << DV_ORIGIN(o);
        value_to_return = false;
        result = "0";
    }
    else
        result = source.simplifyWhiteSpace();

    if (0 != maxInclFacet_.pointer()) {
        if (less(maxInclFacet_->value(), result)) {
            result = maxInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::maxInclFacetConstrain << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != maxExclFacet_.pointer()) {
        if (lessEq(maxExclFacet_->value(), result)) {
            if ( 0 != scale_.pointer())
                shift(maxExclFacet_->value(), false, result);
            else {
                double res;
                if (0 != minInclFacet_.pointer())
                    res = (maxExclFacet_->value().toDouble() +
                           minInclFacet_->value().toDouble() ) / 2;
                else if (0 != minExclFacet_.pointer())
                    res = (maxExclFacet_->value().toDouble() +
                           minExclFacet_->value().toDouble() ) / 2;
                else
                    res = floor(maxExclFacet_->value().toDouble());
                result.setNum(res);
            }
            if (0 != schema)
                schema->mstream() << XsMessages::maxExclFacetConstrain << Message::L_ERROR << maxExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (less(result, minInclFacet_->value())) {
            result = minInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::minInclFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != minExclFacet_.pointer()) {
        if (lessEq(result, minExclFacet_->value())) {
            if ( 0 != scale_.pointer())
                shift(minExclFacet_->value(), true, result);
            else {
                double res;
                if (0 != maxInclFacet_.pointer())
                    res = (minExclFacet_->value().toDouble() +
                           maxInclFacet_->value().toDouble() ) / 2;
                else if (0 != maxExclFacet_.pointer())
                    res = (minExclFacet_->value().toDouble() +
                           maxExclFacet_->value().toDouble() ) / 2;
                else
                    res = ceil(minExclFacet_->value().toDouble());
                result.setNum(res);
            }
            if (0 != schema)
                schema->mstream() << XsMessages::minExclFacetConstrain << Message::L_ERROR
                        << minExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    //! validate on containing digits, sign and decimal point only.
    result = result.stripWhiteSpace();
    for(uint i = 0; i < result.length(); i++) {
        if ((!result.at(i).isDigit()) &&
            ('.' != result.at(i)) &&
            ('-' != result.at(i)) &&
            ('+' != result.at(i))) {
            if (0 != schema)
                schema->mstream() << XsMessages::notAllowedDigit
                                  << Message::L_ERROR  << result
                                  << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    if ((!checkScale(true, result)) && (0 != schema)){
        String er;
        er.setNum(scale_->value());
        schema->mstream() << XsMessages::scaleFacetConstrain
                          << Message::L_ERROR  << er
                          << result << DV_ORIGIN(o);
    }
    if ((!checkPrecision(true, result)) && (0 != schema)) {
        String er;
        er.setNum(precision_->value());
        schema->mstream() << XsMessages::precisionFacetConstrain
                          << Message::L_ERROR  << er << result
                          << DV_ORIGIN(o);
    }
    value_to_return &= pattern_->check(schema, result, o, this);
    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    return value_to_return;
}

void NumberType::dump(int) const
{
    std::cerr << NOTR("NumberType") << std::endl;
}

AtomicType::Type NumberType::type() const
{
    return AtomicType::NUMBER;
}

bool NumberType::setPrecision(Schema* schema, TotalDigitsFacet* precision)
{
    if(0 != precision_.pointer()) {
        if(precision_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("precision") << XS_ORIGIN;
            return false;
        }
    }
    precision_ = precision;

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (!checkPrecision(false, (*i)->value())) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("precision") << NOTR("enumeration") << XS_ORIGIN;
                precision_ = 0;
                return false;
            }
        }
    }

    bool is_ok;

    if (0 != maxInclFacet_.pointer()) {
        is_ok = checkPrecision(false, maxInclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("precision") << NOTR("maxInclusive") << XS_ORIGIN;
             precision_ = 0;
             return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        is_ok = checkPrecision(false, maxExclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("precision") << NOTR("maxExclusive") << XS_ORIGIN;
             precision_ = 0;
             return false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        is_ok = checkPrecision(false, minInclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("precision") << NOTR("minInclusive") << XS_ORIGIN;
             precision_ = 0;
             return false;
        }
    }

    if (0 != minExclFacet_.pointer()) {
        is_ok = checkPrecision(false, minExclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("precision") << NOTR("minExclusive") << XS_ORIGIN;
             precision_ = 0;
             return false;
        }
    }

    return true;
}

bool NumberType::setScale(Schema* schema, FractionDigitsFacet* scale)
{
    //! Null schema is passed from derived datatypes.
    if (0 == schema) {
        scale_ = scale;
        return true;
    }

    if(0 != scale_.pointer()) {
        if(scale_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("scale") << XS_ORIGIN;
            return false;
        }
    }

    scale_ = scale;
    bool is_ok;

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (!checkScale(false, (*i)->value())) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("scale") << NOTR("enumeration") << XS_ORIGIN;
                scale_ = 0;
                return false;
            }
        }
    }

    if (0 != maxInclFacet_.pointer()) {
        is_ok = checkScale(false, maxInclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("scale") << NOTR("maxInclusive") << XS_ORIGIN;
             scale_ = 0;
             return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        is_ok = checkScale(false, maxExclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("scale") << NOTR("maxExclusive") << XS_ORIGIN;
             scale_ = 0;
             return false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        is_ok = checkScale(false, minInclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("scale") << NOTR("minInclusive") << XS_ORIGIN;
             scale_ = 0;
             return false;
        }
    }

    if (0 != minExclFacet_.pointer()) {
        is_ok = checkScale(false, minExclFacet_->value());
        if(!is_ok) {
             if (0 != schema)
                 schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("scale") << NOTR("minExclusive") << XS_ORIGIN;
             scale_ = 0;
             return false;
        }
    }

    return true;
}

bool NumberType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    bool is_ok;
    maxInclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << maxInclFacet->value() << XS_ORIGIN;
        return false;
    }

    is_ok = checkScale(false, maxInclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive") << NOTR("scale") << XS_ORIGIN;
        return false;
    }

    is_ok = checkPrecision(false, maxInclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive") << NOTR("precision") << XS_ORIGIN;
        return false;
    }

    if(0 != maxInclFacet_.pointer()) {
        if(maxInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive") << NOTR("maxExclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (less(maxInclFacet->value(), (*i)->value())) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (less(maxInclFacet->value(),minInclFacet_->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                    << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minExclFacet_.pointer()) {
        if (lessEq(maxInclFacet->value(), minExclFacet_->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                    << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxInclFacet_ = maxInclFacet;
    return true;
}

bool NumberType::setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet)
{
    bool is_ok;
    maxExclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << maxExclFacet->value() << XS_ORIGIN;
        return false;
    }

    is_ok = checkScale(false, maxExclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive") << NOTR("scale") << XS_ORIGIN;
        return false;
    }

    is_ok = checkPrecision(false, maxExclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive") << NOTR("precision") << XS_ORIGIN;
        return false;
    }

    if(0 != maxExclFacet_.pointer()) {
        if(maxExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive") << NOTR("maxInclusive") << XS_ORIGIN;
        return false;
    }


    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (lessEq(maxExclFacet->value(), (*i)->value())) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (lessEq(maxExclFacet->value(), minInclFacet_->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (lessEq(maxExclFacet->value(), minExclFacet_->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxExclusive")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxExclFacet_ = maxExclFacet;
    return true;
}

bool NumberType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    bool is_ok;
    minInclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << minInclFacet->value() << XS_ORIGIN;
        return false;
    }

    is_ok = checkScale(false, minInclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive") << NOTR("scale") << XS_ORIGIN;
        return false;
    }

    is_ok = checkPrecision(false, minInclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive") << NOTR("precision") << XS_ORIGIN;
        return false;
    }

    if(0 != minInclFacet_.pointer()){
        if(minInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive") << NOTR("minExclusive") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (less((*i)->value(), minInclFacet->value())) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (less(maxInclFacet_->value(), minInclFacet->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                    << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (lessEq(maxExclFacet_->value(), minInclFacet->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minInclFacet_ = minInclFacet;
    return true;
}

bool NumberType::setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet)
{
    bool is_ok;
    minExclFacet->value().toDouble(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << minExclFacet->value() << XS_ORIGIN;
        return false;
    }

    is_ok = checkScale(false, minExclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive") << NOTR("scale") << XS_ORIGIN;
        return false;
    }

    is_ok = checkPrecision(false, minExclFacet->value());
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive") << NOTR("precision") << XS_ORIGIN;
        return false;
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (lessEq((*i)->value(), minExclFacet->value())) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if(minExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive") << NOTR("minInclusive") << XS_ORIGIN;
        return false;
    }

    if(0 != maxInclFacet_.pointer()) {
        if (lessEq(maxInclFacet_->value(), minExclFacet->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (lessEq(maxExclFacet_->value(), minExclFacet->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minExclFacet_ = minExclFacet;
    return true;
}


bool NumberType::addPattern(Schema* schema, PatternFacet* pattern)
{
    //TODO:: check for valid pattern.
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool NumberType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    bool is_ok;
    test.toDouble(&is_ok);
    if (!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notDouble << Message::L_ERROR << test << XS_ORIGIN;
        return false;
    }

    is_ok = checkScale(false, test);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("scale") << XS_ORIGIN;
        return false;
    }

    is_ok = checkPrecision(false, test);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("precision") << XS_ORIGIN;
        return false;
    }

    if (0 != maxInclFacet_.pointer()) {
        if (less(maxInclFacet_->value(), test)) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (lessEq(maxExclFacet_->value(), test)) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minInclFacet_.pointer()) {
        if (less(test, minInclFacet_->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (lessEq(test, minExclFacet_->value())) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool NumberType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(NumberType)

/*****************************************************************************
 * QNameType                                                                 *
 *****************************************************************************/

QNameType::QNameType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool QNameType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::LENGTH:
            return setLength     (schema, facet->asLengthFacet());
        case XsFacet::MINLENGTH:
            return setMinLength   (schema, facet->asMinLengthFacet());
        case XsFacet::MAXLENGTH:
            return setMaxLength   (schema, facet->asMaxLengthFacet());
        case XsFacet::MAXINCLUSIVE:
            return setMaxInclusive(schema, facet->asMaxInclusiveFacet());
        case XsFacet::MAXEXCLUSIVE:
            return setMaxExclusive(schema, facet->asMaxExclusiveFacet());
        case XsFacet::MININCLUSIVE:
            return setMinInclusive(schema, facet->asMinInclusiveFacet());
        case XsFacet::MINEXCLUSIVE:
            return setMinExclusive(schema, facet->asMinExclusiveFacet());
        case XsFacet::PATTERN:
            return addPattern     (schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}


bool QNameType::validate(Schema* schema, const GroveLib::Node* o, const String& source,
                          String& result) const
{
    bool value_to_return = true;
    result = source.simplifyWhiteSpace();
    uint length = source.length();
    if (0 != lenFacet_.pointer()) {
        if (length != lenFacet_->value()) {
            //! Bool flag means: true - to cut unneccecary
            // simbols from the end, false - to add spaces to
            // the end of the string to get neccecary length.
            result = source.leftJustify(lenFacet_->value(),' ', true);
            String er;
            er.setNum(lenFacet_->value());
            if (0 != schema)
                schema->mstream() << XsMessages::lengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else {
        if (0 != minLenFacet_.pointer()) {
            if (length < minLenFacet_->value()) {
                result = source.leftJustify(minLenFacet_->value(),' ', true);
                String er;
                er.setNum(minLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::minLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        if (0 != maxLenFacet_.pointer()) {
            if (length > maxLenFacet_->value()) {
                result = source.leftJustify(maxLenFacet_->value(),' ', true);
                String er;
                er.setNum(maxLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::maxLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);

    int end = 0;
    int start = 0;
    while (result.length() > ulong(start)) {
        end = result.find(' ', start);
        if (-1 == end)
            end = result.length();
        String temp = result.mid(start, end - start);

        GROVE_NAMESPACE::QualifiedName qname;
        qname.parse(temp);

        if (qname.isQualified()) {
            const String& qpfx = qname.prefix();
            if (starts_with(qpfx, NOTR("xml")) || ((!qpfx[0].isLetter()) &&
                ('_' != qpfx[0]))) {
                if (0 != schema)
                    schema->mstream() << XsMessages::notNCNamePrefix
                                      << Message::L_ERROR << temp
                                      << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        const String& qln = qname.localName();
        if (!is_null(qln)) {
            if (starts_with(qln, NOTR("xml")) || ((!qln[0].isLetter()) &&
                 ('_' != qln[0]))) {
                if (0 != schema)
                    schema->mstream() << XsMessages::notNCNameLocalName
                                      << Message::L_ERROR << temp
                                      << DV_ORIGIN(o);
                value_to_return = false;
                //TODO:change result.
            }
        }

        if (0 < enumeration_->size()) {
            EnumerationContainer::const_iterator i;
            bool isMatched = false;
            for (i = enumeration_->begin(); i != enumeration_->end(); ++i) {
                if ( (*i)->value() == temp)
                    isMatched = true;
            }
            if (!isMatched) {
                if (0 != schema)
                    schema->mstream() << XsMessages::enumerationFacetConstrain
                                      << Message::L_ERROR << result
                                      << DV_ORIGIN(o);
                value_to_return = false;
                break;
            }
        }
        start = end + 1;
    }

    if (0 == result.length()) {
        if (0 != schema)
            schema->mstream() << XsMessages::valueIsEmpty
                              << Message::L_ERROR  << DV_ORIGIN(o);
        value_to_return = false;
    }

    return value_to_return;
}

void QNameType::dump(int indent) const
{
    (void) indent;
    std::cerr << NOTR("QNameType value::") << std::endl;
}

AtomicType::Type QNameType::type() const
{
    return AtomicType::QNAME;
}

bool QNameType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    //TODO:
    (void) schema;
    (void) maxInclFacet;
    return false;
}

bool QNameType::setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet)
{
    //TODO:
    (void) schema;
    (void) maxExclFacet;
    return false;
}

bool QNameType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    //TODO:
    (void) schema;
    (void) minInclFacet;
    return false;
}

bool QNameType::setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet)
{
    //TODO:
    (void) schema;
    (void) minExclFacet;
    return false;
}

bool QNameType::setLength(Schema* schema, LengthFacet* lenFacet)
{
    if(0 != lenFacet_.pointer()){
        if(lenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }

    if ((0 == minLenFacet_.pointer()) && (0 == maxLenFacet_.pointer())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else if (0 == minLenFacet_.pointer()) {
        if (maxLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    else if (0 == maxLenFacet_.pointer()) {
        if (minLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    else if ((maxLenFacet_->value() == lenFacet->value()) &&
             (minLenFacet_->value() == lenFacet->value())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength and maxLength") << XS_ORIGIN;
        return false;
    }
}

bool QNameType::setMinLength(Schema* schema, MinLengthFacet* minLenFacet)
{
    if(0 != minLenFacet_.pointer()){
        if(minLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    minLenFacet_ = minLenFacet;
    return true;
}

bool QNameType::setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet)
{
    if(0 != maxLenFacet_.pointer()){
        if(maxLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != maxLenFacet->value()){
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > maxLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    maxLenFacet_ = maxLenFacet;
    return true;
}

bool QNameType::addPattern(Schema* schema, PatternFacet* pattern)
{
    (void) schema;
    //TODO:: check for valid pattern.
    pattern_->insert(pattern);
    return true;
}

bool QNameType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    uint len = test.length();
    String result = test.simplifyWhiteSpace();
    int end = 0;
    int start = 0;
    while (result.length() > ulong(start)) {
        end = result.find(' ', start);
        if (-1 == end)
            end = result.length();
        String temp = result.mid(start, end - start);
        GROVE_NAMESPACE::QualifiedName qname;
        qname.parse(temp);

        if (qname.isQualified()) {
            if ((NOTR("xml") == qname.prefix().left(3)) ||
                ((!qname.prefix()[0].isLetter()) && ('_' != qname.prefix()[0]))) {
                if (0 != schema)
                        schema->mstream() << XsMessages::notNCNamePrefix << Message::L_ERROR << temp << XS_ORIGIN;
                return false;
            }
        }
        if (!qname.localName().isNull()) {
            if ((NOTR("xml") == qname.localName().left(3)) ||
                ((!qname.localName()[0].isLetter()) &&
                 ('_' != qname.localName()[0]))) {
                if (0 != schema)
                        schema->mstream() << XsMessages::notNCNameLocalName << Message::L_ERROR << temp << XS_ORIGIN;
                return false;
            }
        }

        start = end + 1;
    }
    if (0 == result.length()) {
        if (0 != schema)
                    schema->mstream() << XsMessages::valueIsEmpty
                                      << Message::L_ERROR  << XS_ORIGIN;
        return false;
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool QNameType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(QNameType)

/*****************************************************************************
 * StringType                                                                *
 *****************************************************************************/

StringType::StringType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived class to
                know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    setWhiteSpace(0, new WhiteSpaceFacet(WhiteSpaceFacet::PRESERVE));
    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool StringType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::LENGTH:
            return setLength(schema, facet->asLengthFacet());
        case XsFacet::MINLENGTH:
            return setMinLength(schema, facet->asMinLengthFacet());
        case XsFacet::MAXLENGTH:
            return setMaxLength(schema, facet->asMaxLengthFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

void StringType::whiteSpacePrepare(const String& source, String& result) const
{
    if (WhiteSpaceFacet::REPLACE == whiteSpaceFacet_->value()) {
        result = source;
        String s = NOTR("[\t\n\r]"); //'[' + QChar(0x9) + QChar(0xA) + QChar(0xD) + ']';
        result.replace( QRegExp(s), " ");
    } else if (WhiteSpaceFacet::COLLAPSE == whiteSpaceFacet_->value()) {
        result = source.simplifyWhiteSpace();
    } else
        result = source;
}

bool StringType::validate(Schema* schema,
                          const GroveLib::Node* o,
                          const String& source,
                          String& result) const
{
    bool value_to_return = true;
    whiteSpacePrepare(source, result);

    value_to_return &= pattern_->check(schema, result, o, this);

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    uint length = result.length();
    if (0 != lenFacet_.pointer()) {
        if (length != lenFacet_->value()) {
            //! Bool flag means: true - to cut unneccecary
            // simbols from the end, false - to add spaces to
            // the end of the string to get neccecary length.
            result = result.leftJustify(lenFacet_->value(),' ', true);
            String er;
            er.setNum(lenFacet_->value());
            if (0 != schema)
                schema->mstream() << XsMessages::lengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else {
        if (0 != minLenFacet_.pointer()) {
            if (length < minLenFacet_->value()) {
                result = result.leftJustify(minLenFacet_->value(),' ', true);
                String er;
                er.setNum(minLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::minLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        if (0 != maxLenFacet_.pointer()) {
            if (length > maxLenFacet_->value()) {
                result = result.leftJustify(maxLenFacet_->value(),' ', true);
                String er;
                er.setNum(maxLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::maxLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
    }

    return value_to_return;
}

void StringType::dump(int indent) const
{
    (void) indent;
    std::cerr << NOTR("StringType value")  << std::endl;
}

AtomicType::Type StringType::type() const
{
    return AtomicType::STRING;
}

bool StringType::setLength(Schema* schema, LengthFacet* lenFacet)
{
    if(0 != lenFacet_.pointer()){
        if(lenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() != lenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("length") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if ((0 == minLenFacet_.pointer()) && (0 == maxLenFacet_.pointer())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else if (0 == minLenFacet_.pointer()) {
        if (maxLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    else if (0 == maxLenFacet_.pointer()) {
        if (minLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    else if ((maxLenFacet_->value() == lenFacet->value()) &&
             (minLenFacet_->value() == lenFacet->value())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength and maxLength") << XS_ORIGIN;
        return false;
    }
}

bool StringType::setMinLength(Schema* schema, MinLengthFacet* minLenFacet)
{
    if(0 != minLenFacet_.pointer()){
        if(minLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() < minLenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("minLength") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    minLenFacet_ = minLenFacet;
    return true;
}

bool StringType::setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet)
{
    if(0 != maxLenFacet_.pointer()){
        if(maxLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() > maxLenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("maxLength") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != maxLenFacet->value()){
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > maxLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    maxLenFacet_ = maxLenFacet;
    return true;
}

bool StringType::addPattern(Schema* schema, PatternFacet* pattern)
{
    (void) schema;
    //TODO:: check for valid pattern.
    pattern_->insert(pattern);
    return true;
}

bool StringType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    uint len = test.length();
    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    enumeration_->insert(enumeration);
    return true;
}

bool StringType::setWhiteSpace(Schema* schema, WhiteSpaceFacet* whiteSpaceFacet)
{
    if (0 != whiteSpaceFacet_.pointer()) {
        if (whiteSpaceFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
            return false;
        }
    }
    whiteSpaceFacet_ = whiteSpaceFacet;
    return true;
}

CLONE_ATOMIC_TYPE(StringType)

/*****************************************************************************
 * TimeType                                                                  *
 *****************************************************************************/

TimeType::TimeType(const Origin& origin, const NcnCred& cred)
    : RecurringType(origin, cred)
{
    String period = NOTR("P1D");
    String duration = NOTR("P0Y");
    setPeriod(0, new PeriodFacet(period, true));
    setDuration(0, new DurationFacet(duration, true));
}

void TimeType::formLexicalRepresentation(String& result,
                                         bool sign,
                                         String&,
                                         String&,
                                         String&,
                                         String& hour,
                                         String& minute,
                                         String& sec,
                                         String& msec) const
{
    //TODO: msec
    (void) msec;
    result = hour + ':' + minute + ':' + sec;
    if (!sign)
        result = '-' + result;
}

void TimeType::dump(int) const
{
    std::cerr << NOTR("TimeType") << std::endl;
}

AtomicType::Type TimeType::type() const
{
    return AtomicType::TIME;
}

CLONE_ATOMIC_TYPE(TimeType)

/*****************************************************************************
 * YearMontType                                                              *
 *****************************************************************************/

YearMonthType::YearMonthType(const Origin& origin,
                             const NcnCred& cred)
    : TimePeriodType(origin, cred)
{
    String duration = NOTR("P1M");
    setDuration(0, new DurationFacet(duration, true));
}

void YearMonthType::formLexicalRepresentation(String& result,
                                          bool sign,
                                          String& year,
                                          String& month,
                                          String& ,
                                          String& ,
                                          String& ,
                                          String& ,
                                          String& ) const
{
    result = year + '-' + month;
    if (!sign)
        result = '-' + result;
}

void YearMonthType::dump(int) const
{
    std::cerr << NOTR("YearMonthType") << std::endl;
}

AtomicType::Type YearMonthType::type() const
{
    return AtomicType::YEARMONTH;
}

CLONE_ATOMIC_TYPE(YearMonthType)

/*****************************************************************************
 * YearType                                                                  *
 *****************************************************************************/
YearType::YearType(const Origin& origin,
                   const NcnCred& cred)
    : TimePeriodType(origin, cred)
{
    String duration = NOTR("P1Y");
    setDuration(0, new DurationFacet(duration, true));
}

void YearType::formLexicalRepresentation(String& result,
                                         bool sign,
                                         String& year,
                                         String& ,
                                         String& ,
                                         String& ,
                                         String& ,
                                         String& ,
                                         String& ) const
{
    result = year;
    if (!sign)
        result = '-' + result;
}

void YearType::dump(int) const
{
    std::cerr << NOTR("YearType") << std::endl;
}

AtomicType::Type YearType::type() const
{
    return AtomicType::YEAR;
}

CLONE_ATOMIC_TYPE(YearType)

/*****************************************************************************
 * BooleanType                                                               *
 *****************************************************************************/
BooleanType::BooleanType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    pattern_ = new PatternContainer;
}

bool BooleanType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}


bool BooleanType::validate(Schema* schema, const GroveLib::Node* o, const String& source,
                           String& result) const
{

    static String is_true = NOTR("true");
    static String is_false = NOTR("false");
    bool value_to_return = true;
    result = source.simplifyWhiteSpace();
    bool is_ok;
    int v = source.toInt(&is_ok);
    if (is_ok) {
        if ((0 != v) && (1 != v)) {
            if (0 != schema)
                schema->mstream() << XsMessages::notBool << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else if ((is_true != result.lower()) && (is_false != result.lower())) {
        value_to_return = false;
        if (0 != schema)
            schema->mstream() << XsMessages::notBool << Message::L_ERROR << result << DV_ORIGIN(o);
        result = is_false;
    }
    value_to_return &= pattern_->check(schema, result, o, this);
    return value_to_return;
}

void BooleanType::dump(int indent) const
{
    (void) indent;
    std::cerr << NOTR("BooleanType value::") << std::endl;
}

AtomicType::Type BooleanType::type() const
{
    return AtomicType::BOOLEAN;
}


bool BooleanType::addPattern(Schema* schema, PatternFacet* pattern)
{
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool BooleanType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

bool BooleanType::addEnumeration(Schema*, EnumerationFacet*)
{
    return false;
}

CLONE_ATOMIC_TYPE(BooleanType)

/*****************************************************************************
 * FloatType                                                                  *
 *****************************************************************************/

FloatType::FloatType(  const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool FloatType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::MAXINCLUSIVE:
            return setMaxInclusive(schema, facet->asMaxInclusiveFacet());
        case XsFacet::MAXEXCLUSIVE:
            return setMaxExclusive(schema, facet->asMaxExclusiveFacet());
        case XsFacet::MININCLUSIVE:
            return setMinInclusive(schema, facet->asMinInclusiveFacet());
        case XsFacet::MINEXCLUSIVE:
            return setMinExclusive(schema, facet->asMinExclusiveFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

bool FloatType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool is_ok;
    bool value_to_return = true;
    float val = source.toFloat(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notFloat << Message::L_ERROR << source << DV_ORIGIN(o);
        value_to_return = false;
        result = "0";
    }
    else
        result = source.simplifyWhiteSpace();

    if (0 != maxInclFacet_.pointer()) {
        if (val > maxInclFacet_->value().toFloat()) {
            result = maxInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::maxInclFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != maxExclFacet_.pointer()) {
        if (val >= maxExclFacet_->value().toFloat()) {
            float res;
            if (0 != minInclFacet_.pointer())
                res = (maxExclFacet_->value().toFloat() +
                       minInclFacet_->value().toFloat() ) / 2;
            else if (0 != minExclFacet_.pointer())
                res = (maxExclFacet_->value().toFloat() +
                       minExclFacet_->value().toFloat() ) / 2;
            else
                res = floor(maxExclFacet_->value().toFloat());
            result.setNum(res);
            if (0 != schema)
                schema->mstream() << XsMessages::maxExclFacetConstrain << Message::L_ERROR
                        << maxExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (val < minInclFacet_->value().toFloat()) {
            result = minInclFacet_->value();
            if (0 != schema)
                schema->mstream() << XsMessages::minInclFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else  if (0 != minExclFacet_.pointer()) {
        if (val <= minExclFacet_->value().toFloat()) {
            float res;
            if (0 != maxInclFacet_.pointer())
                res = (minExclFacet_->value().toFloat() +
                       maxInclFacet_->value().toFloat() ) / 2;
            else if (0 != maxExclFacet_.pointer())
                res = (minExclFacet_->value().toFloat() +
                       maxExclFacet_->value().toFloat() ) / 2;
            else
                res = ceil(minExclFacet_->value().toFloat());
            result.setNum(res);
            if (0 != schema)
                schema->mstream() << XsMessages::minExclFacetConstrain << Message::L_ERROR
                        << minExclFacet_->value() << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);
    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    return value_to_return;
}

void FloatType::dump(int) const
{
    std::cerr << NOTR("FloatType") << std::endl;
}

AtomicType::Type FloatType::type() const
{
    return AtomicType::FLOAT;
}

bool FloatType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    bool is_ok;
    maxInclFacet->value().toFloat(&is_ok);
    if (!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notFloat << Message::L_ERROR << maxInclFacet->value() << XS_ORIGIN;
        return false;
    }

    if (0 != maxInclFacet_.pointer()){
        if(maxInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("maxInclusive") << NOTR("maxExclusive") << XS_ORIGIN;
        return false;
    }

    if (!enumeration_->empty()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (maxInclFacet->value().toFloat() < (*i)->value().toFloat() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("maxInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (maxInclFacet->value().toFloat() < minInclFacet_->value().toFloat() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }
    if (0 != minExclFacet_.pointer()) {
        if (maxInclFacet->value().toFloat() <= minExclFacet_->value().toFloat() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxInclusive")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxInclFacet_ = maxInclFacet;
    return true;
}

bool FloatType::setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet)
{
    bool is_ok;
    maxExclFacet->value().toFloat(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notFloat << Message::L_ERROR << maxExclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != maxExclFacet_.pointer()) {
        if(maxExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("maxExclusive") << NOTR("maxInclusive") << XS_ORIGIN;
        return false;
    }

    if (!enumeration_->empty()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if (maxExclFacet->value().toFloat() <= (*i)->value().toFloat() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("maxExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (maxExclFacet->value().toFloat() <=
            minInclFacet_->value().toFloat()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("maxExclusive")
                                  << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minExclFacet_.pointer()) {
        if (maxExclFacet->value().toFloat() <=
            minExclFacet_->value().toFloat()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict
                                  << Message::L_ERROR << NOTR("maxExclusive")
                                  << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    maxExclFacet_ = maxExclFacet;
    return true;
}

bool FloatType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    bool is_ok;
    minInclFacet->value().toFloat(&is_ok);
    if(!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notFloat << Message::L_ERROR << minInclFacet->value() << XS_ORIGIN;
        return false;
    }

    if(0 != minInclFacet_.pointer()){
        if(minInclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != minExclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("minInclusive") << NOTR("minExclusive") << XS_ORIGIN;
        return false;
    }

    if (!enumeration_->empty()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ((*i)->value().toFloat() < minInclFacet->value().toFloat() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("minInclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclFacet_->value().toFloat() < minInclFacet->value().toFloat() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        if (maxExclFacet_->value().toFloat() <= minInclFacet->value().toFloat() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minInclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minInclFacet_ = minInclFacet;
    return true;
}

bool FloatType::setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet)
{
    bool is_ok;
    minExclFacet->value().toFloat(&is_ok);
    if (!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notFloat << Message::L_ERROR << minExclFacet->value() << XS_ORIGIN;
        return false;
    }

    if (0 != minExclFacet_.pointer()) {
        if(minExclFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("minExclusive") << NOTR("minInclusive") << XS_ORIGIN;
        return false;
    }

    if (!enumeration_->empty()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ((*i)->value().toFloat() <= minExclFacet->value().toFloat() ) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            << NOTR("minExclusive") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != maxInclFacet_.pointer()) {
        if (maxInclFacet_->value().toFloat() <= minExclFacet->value().toFloat() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != maxExclFacet_.pointer()) {
        if (maxExclFacet_->value().toFloat() <= minExclFacet->value().toFloat() ) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minExclusive")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    minExclFacet_ = minExclFacet;
    return true;
}


bool FloatType::addPattern(Schema* schema, PatternFacet* pattern)
{
    //TODO:: check for valid pattern.
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool FloatType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    bool is_ok;
    float val = test.toFloat(&is_ok);
    if (!is_ok) {
        if (0 != schema)
            schema->mstream() << XsMessages::notFloat << Message::L_ERROR << test << XS_ORIGIN;
        return false;
    }

    if (0 != maxInclFacet_.pointer()) {
        if (maxInclFacet_->value().toFloat() < val) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != maxExclFacet_.pointer()) {
        if (maxExclFacet_->value().toFloat() <= val) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("maxExclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minInclFacet_.pointer()) {
        if (val < minInclFacet_->value().toFloat()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minInclusive") << XS_ORIGIN;
            return false;
        }
    }

    if (0 != minExclFacet_.pointer()) {
        if (val <= minExclFacet_->value().toFloat()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration")
                        << NOTR("minExclusive") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool FloatType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(FloatType)

/*****************************************************************************
 * IDType                                                                    *
 *****************************************************************************/

IDType::IDType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool IDType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::LENGTH:
            return setLength     (schema, facet->asLengthFacet());
        case XsFacet::MINLENGTH:
            return setMinLength   (schema, facet->asMinLengthFacet());
        case XsFacet::MAXLENGTH:
            return setMaxLength   (schema, facet->asMaxLengthFacet());
        case XsFacet::MAXINCLUSIVE:
            return setMaxInclusive(schema, facet->asMaxInclusiveFacet());
        case XsFacet::MAXEXCLUSIVE:
            return setMaxExclusive(schema, facet->asMaxExclusiveFacet());
        case XsFacet::MININCLUSIVE:
            return setMinInclusive(schema, facet->asMinInclusiveFacet());
        case XsFacet::MINEXCLUSIVE:
            return setMinExclusive(schema, facet->asMinExclusiveFacet());
        case XsFacet::PATTERN:
            return addPattern     (schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet
                                  << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

typedef Range<Char*> CRange;

inline bool operator==(const EnumerationContainer::value_type& lhs,
                       const CRange& rhs)
{
    return lhs->value() == rhs;
}

inline bool operator!=(const EnumerationContainer::value_type& lhs,
                       const CRange& rhs)
{
    return !(lhs == rhs);
}

bool IDType::validate(Schema* schema, const GroveLib::Node* o, const String& source,
                      String& result) const
{
    bool value_to_return = true;
    result = source.simplifyWhiteSpace();

    uint length = source.length();
    if (0 != lenFacet_.pointer()) {
        if (length != lenFacet_->value()) {
            //! Bool flag means: true - to cut unneccecary
            // simbols from the end, false - to add spaces to
            // the end of the string to get neccecary length.
            result = source.leftJustify(lenFacet_->value(),' ', true);
            String er;
            er.setNum(lenFacet_->value());
            if (0 != schema)
                schema->mstream() << XsMessages::lengthFacetConstrain
                                  << Message::L_ERROR << er << result
                                  << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else {
        if (0 != minLenFacet_.pointer()) {
            if (length < minLenFacet_->value()) {
                result = source.leftJustify(minLenFacet_->value(), ' ', true);
                String er;
                er.setNum(minLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::minLengthFacetConstrain
                                      << Message::L_ERROR << er << result
                                      << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        if (0 != maxLenFacet_.pointer()) {
            if (length > maxLenFacet_->value()) {
                result = source.leftJustify(maxLenFacet_->value(),' ', true);
                String er;
                er.setNum(maxLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::maxLengthFacetConstrain
                                      << Message::L_ERROR << er << result
                                      << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);

    String s = result;
    if (!check_id(schema, o, s, result))
        value_to_return = false;

    IsSpace<Char> wspr;
    CRange r(find_first_range_not_of(result.begin(), result.end(), wspr));
    while (!r.empty()) {
        if (!enumeration_->empty()) {
            EnumerationContainer::const_iterator i;
            i = std::find(enumeration_->begin(), enumeration_->end(), r);
            if (i == enumeration_->end()) {
                result = enumeration_->front()->value();
                if (0 != schema)
                    schema->mstream() << XsMessages::enumerationFacetConstrain
                                      << Message::L_ERROR
                                      << result << DV_ORIGIN(o);
                value_to_return = false;
                break;
            }
        }
        r = find_first_range_not_of(r.end(), result.end(), wspr);
    }

    if (0 == result.length()) {
        if (0 != schema)
            schema->mstream() << XsMessages::valueIsEmpty
                              << Message::L_ERROR
                              << DV_ORIGIN(o);
        value_to_return = false;
    }

    return value_to_return;
}

void IDType::dump(int indent) const
{
    (void) indent;
    std::cerr << NOTR("IDType value") << std::endl;
}

AtomicType::Type IDType::type() const
{
    return AtomicType::ID;
}

bool IDType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    //TODO:
    (void) schema;
    (void) maxInclFacet;
    return false;
}

bool IDType::setMaxExclusive(Schema* schema, MaxExclusiveFacet* maxExclFacet)
{
    //TODO:
    (void) schema;
    (void) maxExclFacet;
    return false;
}

bool IDType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    //TODO:
    (void) schema;
    (void) minInclFacet;
    return false;
}

bool IDType::setMinExclusive(Schema* schema, MinExclusiveFacet* minExclFacet)
{
    //TODO:
    (void) schema;
    (void) minExclFacet;
    return false;
}

bool IDType::setLength(Schema* schema, LengthFacet* lenFacet)
{
    if(0 != lenFacet_.pointer()){
        if(lenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }

    if ((0 == minLenFacet_.pointer()) && (0 == maxLenFacet_.pointer())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else if (0 == minLenFacet_.pointer()) {
        if (maxLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    else if (0 == maxLenFacet_.pointer()) {
        if (minLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    else if ((maxLenFacet_->value() == lenFacet->value()) &&
             (minLenFacet_->value() == lenFacet->value())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength and maxLength") << XS_ORIGIN;
        return false;
    }
}

bool IDType::setMinLength(Schema* schema, MinLengthFacet* minLenFacet)
{
    if(0 != minLenFacet_.pointer()){
        if(minLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    minLenFacet_ = minLenFacet;
    return true;
}

bool IDType::setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet)
{
    if(0 != maxLenFacet_.pointer()){
        if(maxLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != maxLenFacet->value()){
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > maxLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    maxLenFacet_ = maxLenFacet;
    return true;
}

bool IDType::addPattern(Schema* schema, PatternFacet* pattern)
{
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool IDType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    uint len = test.length();
    String result = test.simplifyWhiteSpace();

    if (result.empty()) {
        if (0 != schema)
            schema->mstream() << XsMessages::valueIsEmpty << Message::L_ERROR
                              << XS_ORIGIN;
        return false;
    }

    if (0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if (0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    if (0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool IDType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(IDType)

/*****************************************************************************
 * IDREFType                                                                    *
 *****************************************************************************/

IDREFType::IDREFType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool IDREFType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::LENGTH:
            return setLength     (schema, facet->asLengthFacet());
        case XsFacet::MINLENGTH:
            return setMinLength   (schema, facet->asMinLengthFacet());
        case XsFacet::MAXLENGTH:
            return setMaxLength   (schema, facet->asMaxLengthFacet());
        case XsFacet::PATTERN:
            return addPattern     (schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}


bool IDREFType::validate(Schema* schema, const GroveLib::Node* o,
                         const String& source, String& result) const
{
    bool value_to_return = true;
    result = source.simplifyWhiteSpace();

    uint length = source.length();
    if (0 != lenFacet_.pointer()) {
        if (length != lenFacet_->value()) {
            //! Bool flag means: true - to cut unneccecary
            // simbols from the end, false - to add spaces to
            // the end of the string to get neccecary length.
            result = source.leftJustify(lenFacet_->value(), ' ', true);
            String er(from_integer<ustring>(lenFacet_->value()));
//            er.setNum(lenFacet_->value());
            if (0 != schema)
                schema->mstream() << XsMessages::lengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else {
        if (0 != minLenFacet_.pointer()) {
            if (length < minLenFacet_->value()) {
                result = source.leftJustify(minLenFacet_->value(),' ', true);
                String er;
                er.setNum(minLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::minLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        if (0 != maxLenFacet_.pointer()) {
            if (length > maxLenFacet_->value()) {
                result = source.leftJustify(maxLenFacet_->value(), ' ', true);
                String er;
                er.setNum(maxLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::maxLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);

    String s = result;
    if (!check_id(schema, o, s, result, true))
        value_to_return = false;

//    QStringList list = QStringList::split(' ', result);
//    for (uint i = 0; i < list.count(); i++) {
//        String temp = list[i];
    IsSpace<Char> wspr;
    CRange r(find_first_range_not_of(result.begin(), result.end(), wspr));
    while (!r.empty()) {
        String temp(r.data(), r.size());
        if (!enumeration_->empty()) {
            EnumerationContainer::iterator i;
            for (i = enumeration_->begin(); i != enumeration_->end(); ++i)
                if ((*i)->value() == temp)
                    break;
            if (enumeration_->end() == i) {
                result = enumeration_->front()->value();
                if (0 != schema)
                    schema->mstream() << XsMessages::enumerationFacetConstrain
                                      << Message::L_ERROR << result
                                      << DV_ORIGIN(o);
                value_to_return = false;
                break;
            }
        }
        r = find_first_range_not_of(r.end(), result.end(), wspr);
    }
    if (result.empty()) {
        if (0 != schema)
            schema->mstream() << XsMessages::valueIsEmpty << Message::L_ERROR
                              << DV_ORIGIN(o);
        value_to_return = false;
    }

    return value_to_return;
}

void IDREFType::dump(int indent) const
{
    (void) indent;
    std::cerr << NOTR("IDREFType value") << std::endl;
}

AtomicType::Type IDREFType::type() const
{
    return AtomicType::IDREF;
}

bool IDREFType::setLength(Schema* schema, LengthFacet* lenFacet)
{
    if (0 != lenFacet_.pointer()){
        if(lenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }

    if ((0 == minLenFacet_.pointer()) && (0 == maxLenFacet_.pointer())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else if (0 == minLenFacet_.pointer()) {
        if (maxLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    else if (0 == maxLenFacet_.pointer()) {
        if (minLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    else if ((maxLenFacet_->value() == lenFacet->value()) &&
             (minLenFacet_->value() == lenFacet->value())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength and maxLength") << XS_ORIGIN;
        return false;
    }
}

bool IDREFType::setMinLength(Schema* schema, MinLengthFacet* minLenFacet)
{
    if(0 != minLenFacet_.pointer()){
        if(minLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    minLenFacet_ = minLenFacet;
    return true;
}

bool IDREFType::setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet)
{
    if(0 != maxLenFacet_.pointer()){
        if(maxLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != maxLenFacet->value()){
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > maxLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    maxLenFacet_ = maxLenFacet;
    return true;
}

bool IDREFType::addPattern(Schema* schema, PatternFacet* pattern)
{
    (void) schema;
    pattern_->insert(pattern);
    return true;
}

bool IDREFType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    uint len = test.length();
    String result = test.simplifyWhiteSpace();

    if (0 == result.length()) {
        if (0 != schema)
            schema->mstream() << XsMessages::valueIsEmpty << Message::L_ERROR
                              << XS_ORIGIN;
        return false;
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    enumeration_->insert(enumeration);
    return true;
}

bool IDREFType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(IDREFType)


/*****************************************************************************
 * IDREFSType                                                                    *
 *****************************************************************************/

IDREFSType::IDREFSType(const Origin& origin, const NcnCred& cred)
    : IDREFType(origin, cred)
{
}

bool IDREFSType::validate(Schema* schema,
                          const GroveLib::Node* o,
                          const String& source,
                          String& result) const
{
    bool value_to_return = true;
    result = source.simplifyWhiteSpace();
    uint length = source.length();
    if (0 != lenFacet_.pointer()) {
        if (length != lenFacet_->value()) {
            //! Bool flag means: true - to cut unneccecary
            // simbols from the end, false - to add spaces to
            // the end of the string to get neccecary length.
            result = source.leftJustify(lenFacet_->value(),' ', true);
            String er;
            er.setNum(lenFacet_->value());
            if (0 != schema)
                schema->mstream() << XsMessages::lengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else {
        if (0 != minLenFacet_.pointer()) {
            if (length < minLenFacet_->value()) {
                result = source.leftJustify(minLenFacet_->value(),' ', true);
                String er;
                er.setNum(minLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::minLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        if (0 != maxLenFacet_.pointer()) {
            if (length > maxLenFacet_->value()) {
                result = source.leftJustify(maxLenFacet_->value(),' ', true);
                String er;
                er.setNum(maxLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::maxLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
    }
    value_to_return &= pattern_->check(schema, result, o, this);

    IsSpace<Char> wspr;
    CRange r(find_first_range_not_of(result.begin(), result.end(), wspr));
    while (!r.empty()) {
        String temp(r.data(), r.size());
        String s = temp;
        if (!check_id(schema, o, s, temp, true, true))
            value_to_return = false;

        if (!enumeration_->empty()) {
            EnumerationContainer::const_iterator i;
//            bool isMatched = false;
            for (i = enumeration_->begin(); i != enumeration_->end(); ++i)
                if ((*i)->value() == temp)
                    break;
            if (i == enumeration_->end()) {
                result = enumeration_->front()->value();
                if (0 != schema)
                    schema->mstream() << XsMessages::enumerationFacetConstrain
                                      << Message::L_ERROR << result
                                      << DV_ORIGIN(o);
                value_to_return = false;
                break;
            }
        }
        r = find_first_range_not_of(r.end(), result.end(), wspr);
    }

    if (0 == result.length()) {
        if (0 != schema)
            schema->mstream() << XsMessages::valueIsEmpty << Message::L_ERROR
                              << DV_ORIGIN(o);
        value_to_return = false;
    }

    return value_to_return;
}

void IDREFSType::dump(int indent) const
{
    (void) indent;
    std::cerr << NOTR("IDREFSType value") << std::endl;
}

AtomicType::Type IDREFSType::type() const
{
    return AtomicType::IDREFS;
}

CLONE_ATOMIC_TYPE(IDREFSType)

/*****************************************************************************
 * LanguageType                                                              *
 *****************************************************************************/

LanguageType::LanguageType(const Origin& origin, const NcnCred& cred)
    : TokenType(origin, cred)
{
}

bool LanguageType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool value_to_return = check_lang(source, result);
    if (value_to_return) {
        return TokenType::validate(schema, o, source, result);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notLanguage << Message::L_ERROR << source << DV_ORIGIN(o);
//        String s = NOTR("en");
//        TokenType::validate(schema, o, s, result);
        return false;
    }
}

bool LanguageType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test;
    bool is_ok = check(enumeration->value(), test);
    if (is_ok) {
        return TokenType::addEnumeration(schema, enumeration);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notLanguage << Message::L_ERROR << enumeration->value() << XS_ORIGIN;
        return false;
    }
}

void LanguageType::dump(int) const
{
    std::cerr << NOTR("LanguageType") << std::endl;
}

AtomicType::Type LanguageType::type() const
{
    return AtomicType::LANGUAGE;
}

CLONE_ATOMIC_TYPE(LanguageType)

/*****************************************************************************
 * NCNameType                                                                  *
 *****************************************************************************/

NCNameType::NCNameType(const Origin& origin, const NcnCred& cred)
    : NameType(origin, cred)
{
}

bool NCNameType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool value_to_return = check_nc(source, result);
    if (value_to_return) {
        return NameType::validate(schema, o, source, result);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notNCName << Message::L_ERROR << source << DV_ORIGIN(o);
//        String s = NOTR("NCName");
//        NameType::validate(schema, o, s, result);
        return false;
    }
}

bool NCNameType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test;
    bool is_ok = check_nc(enumeration->value(), test);
    if (is_ok) {
        return NameType::addEnumeration(schema, enumeration);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notNCName << Message::L_ERROR << enumeration->value() << XS_ORIGIN;
        return false;
    }
}

void NCNameType::dump(int) const
{
    std::cerr << NOTR("NCNameType") << std::endl;
}

AtomicType::Type NCNameType::type() const
{
    return AtomicType::NCNAME;
}

CLONE_ATOMIC_TYPE(NCNameType)

/*****************************************************************************
 * TokenType                                                                 *
 *****************************************************************************/

TokenType::TokenType(const Origin& origin,
                     const NcnCred& cred)
    : StringType(origin, cred)
{
}

bool TokenType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool value_to_return = true;
    whiteSpacePrepare(source, result);
    if (result != result.simplifyWhiteSpace()) {
        if (0 != schema)
            schema->mstream() << XsMessages::notToken << Message::L_ERROR << source << DV_ORIGIN(o);
        value_to_return = false;
    }

    String s = result;
    if (value_to_return) {
        return StringType::validate(schema, o, s, result);
    }
    else {
        StringType::validate(schema, o, s, result);
        return false;
    }
}

bool TokenType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    String result = test.simplifyWhiteSpace();
    if (test != result) {
        if (0 != schema)
            schema->mstream() << XsMessages::notToken << Message::L_ERROR << test << XS_ORIGIN;
        return false;
    }

    return StringType::addEnumeration(schema, enumeration);
}

void TokenType::dump(int) const
{
    std::cerr << NOTR("TokenType value::") << value_ << std::endl;
}

AtomicType::Type TokenType::type() const
{
    return AtomicType::TOKEN;
}

CLONE_ATOMIC_TYPE(TokenType)

/*****************************************************************************
 * NmTokenType
 The úvalue spaceú of NMTOKEN is the set of tokens that úmatchú
 the Nmtoken production in [XML 1.0 (Second Edition)] :

{4] NameChar::=ÿLetter | Digit | '.' | '-' | '_' | ':' | CombiningChar | Extender
[5] Name    ::=ÿ(Letter | '_' | ':') ( NameChar)*
[6] Namesÿÿÿ::=ÿName (#x20 Name)*
[7] Nmtokenÿ::=ÿ(NameChar)+
[8]ÿNmtokens::=ÿNmtoken (#x20 Nmtoken)*
 *****************************************************************************/
NmtokenType::NmtokenType(const Origin& origin,
                         const NcnCred& cred)
    : TokenType(origin, cred)
{
}

bool NmtokenType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    bool value_to_return = check_nm(source, result);
    if (value_to_return) {
        return TokenType::validate(schema, o, source, result);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notNmtoken << Message::L_ERROR << source << DV_ORIGIN(o);
//        String s = NOTR("nmtoken");
//        TokenType::validate(schema, o, s, result);
        return false;
    }
}

bool NmtokenType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test;
    bool is_ok = check_nm(enumeration->value(), test);
    if (is_ok) {
        return TokenType::addEnumeration(schema, enumeration);
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::notNmtoken << Message::L_ERROR << enumeration->value() << XS_ORIGIN;
        return false;
    }
}

void NmtokenType::dump(int) const
{
    std::cerr << NOTR("NmtokenType") << std::endl;
}

AtomicType::Type NmtokenType::type() const
{
    return AtomicType::NMTOKEN;
}

CLONE_ATOMIC_TYPE(NmtokenType)

/*****************************************************************************
 * NmTokensType                                                              *
 *****************************************************************************/

NmtokensType::NmtokensType(const Origin& origin,
                           const NcnCred& cred)
    : NmtokenType(origin, cred)
{
}

void NmtokensType::dump(int) const
{
    std::cerr << NOTR("NmtokensType") << std::endl;
}

AtomicType::Type NmtokensType::type() const
{
    return AtomicType::NMTOKENS;
}

CLONE_ATOMIC_TYPE(NmtokensType)

/*****************************************************************************
 * EntityType                                                              *
 *****************************************************************************/

EntityType::EntityType(const Origin& origin,
                       const NcnCred& cred)
    : NmtokenType(origin, cred)
{
}

void EntityType::dump(int) const
{
    std::cerr << NOTR("EntityType") << std::endl;
}

AtomicType::Type EntityType::type() const
{
    return AtomicType::ENTITY;
}

CLONE_ATOMIC_TYPE(EntityType)


/*****************************************************************************
 * NormalizedStringType                                                      *
 *****************************************************************************/

NormalizedStringType::NormalizedStringType(const Origin& origin,
                                           const NcnCred& cred)
    : StringType(origin, cred)
{
}

bool NormalizedStringType::validate(Schema* schema, const GroveLib::Node* o, const String& source, String& result) const
{
    whiteSpacePrepare(source, result);
    if ((result.contains(QChar(0xD)))  ||
        (result.contains(QChar(0xA)))  ||
        (result.contains(QChar(0x9)))) {
        if (0 != schema)
            schema->mstream() << XsMessages::notCDATAValid << Message::L_ERROR << source << DV_ORIGIN(o);
        result = source;
        String s = NOTR("[\t\n\r]"); //'[' + QChar(0x9) + QChar(0xA) + QChar(0xD) + ']';
        result.replace( QRegExp(s), " " );
        String src = result;
        StringType::validate(schema, o, src, result);
        return false;
    }
    String s = result;
    bool valid = StringType::validate(schema, o, s, result);
    if ((result.contains(QChar(0xD)))  ||
        (result.contains(QChar(0xA)))  ||
        (result.contains(QChar(0x9)))) {
        String s = NOTR("[\t\n\r]"); //'[' + QChar(0x9) + QChar(0xA) + QChar(0xD) + ']';
        result.replace( QRegExp(s), "0" );
    }
    return valid;
}

bool NormalizedStringType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    if ((test.contains(QChar(0x0D)))  ||
        (test.contains(QChar(0x0A)))  ||
        (test.contains(QChar(0x09)))) {
        if (0 != schema)
            schema->mstream() << XsMessages::notCDATAValid << Message::L_ERROR << test << XS_ORIGIN;
        return false;
    }
    return StringType::addEnumeration(schema, enumeration);
}

void NormalizedStringType::dump(int) const
{
    std::cerr << NOTR("NormalizedStringType value::") << value_ << std::endl;
}

AtomicType::Type NormalizedStringType::type() const
{
    return AtomicType::NORMALIZEDSTRING;
}

CLONE_ATOMIC_TYPE(NormalizedStringType)

/*****************************************************************************
 * LongType                                                                  *
 *****************************************************************************/

LongType::LongType(const Origin& origin, const NcnCred& cred)
    : IntegerType(origin, cred)
{
    String max;
    max = max.setNum(LONG_MAX);
    String min;
    min = min.setNum(LONG_MIN);
    setMaxInclusive(0, new MaxInclusiveFacet(max));
    setMinInclusive(0, new MinInclusiveFacet(min));
}


bool LongType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    long val = maxInclFacet->value().toLong();
    if (LONG_MAX  < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::longFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return IntegerType::setMaxInclusive(schema, maxInclFacet);
}


bool LongType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    long val = minInclFacet->value().toLong();
    if (LONG_MIN > val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::longFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return IntegerType::setMinInclusive(schema, minInclFacet);
}

void LongType::dump(int) const
{
    std::cerr << NOTR("LongType") << std::endl;
}

AtomicType::Type LongType::type() const
{
    return AtomicType::LONG;
}

CLONE_ATOMIC_TYPE(LongType)


/*****************************************************************************
 * IntType                                                                   *
 *****************************************************************************/

IntType::IntType(const Origin& origin,
                 const NcnCred& cred)
    : LongType(origin, cred)
{
    String max;
    max = max.setNum(INT_MAX);
    String min;
    min = min.setNum(INT_MIN);
    setMaxInclusive(0, new MaxInclusiveFacet(max));
    setMinInclusive(0, new MinInclusiveFacet(min));
}

bool IntType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    int val = maxInclFacet->value().toInt();
    if (INT_MAX < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::intFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return LongType::setMaxInclusive(schema, maxInclFacet);
}


bool IntType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    int val = minInclFacet->value().toInt();
    if (INT_MIN > val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::intFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return LongType::setMinInclusive(schema, minInclFacet);
}

void IntType::dump(int) const
{
    std::cerr << NOTR("IntType") << std::endl;
}

AtomicType::Type IntType::type() const
{
    return AtomicType::INT;
}

CLONE_ATOMIC_TYPE(IntType)

/*****************************************************************************
 * ShortType                                                                  *
 *****************************************************************************/

ShortType::ShortType(const Origin& origin,
                     const NcnCred& cred)
    : IntType(origin, cred)
{
    String max;
    max = max.setNum(SHRT_MAX);
    String min;
    min = min.setNum(SHRT_MIN);
    setMaxInclusive(0, new MaxInclusiveFacet(max));
    setMinInclusive(0, new MinInclusiveFacet(min));
}


bool ShortType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    int val = maxInclFacet->value().toShort();
    if (SHRT_MAX < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::shortFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return IntType::setMaxInclusive(schema, maxInclFacet);
}


bool ShortType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    int val = minInclFacet->value().toShort();
    if (SHRT_MIN > val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::shortFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return IntType::setMinInclusive(schema, minInclFacet);
}

void ShortType::dump(int) const
{
    std::cerr << NOTR("ShortType") << std::endl;
}

AtomicType::Type ShortType::type() const
{
    return AtomicType::SHORT;
}

CLONE_ATOMIC_TYPE(ShortType)

/*****************************************************************************
 * ByteType                                                                  *
 *****************************************************************************/

ByteType::ByteType(const Origin& origin,
                   const NcnCred& cred)
    : ShortType(origin, cred)
{
    String max = NOTR("127");
    String min = NOTR("-128");
    setMaxInclusive(0, new MaxInclusiveFacet(max));
    setMinInclusive(0, new MinInclusiveFacet(min));
}

bool ByteType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    int val = maxInclFacet->value().toShort();
    if (127 < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::byteFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return ShortType::setMaxInclusive(schema, maxInclFacet);
}


bool ByteType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    int val = minInclFacet->value().toShort();
    if (-128 > val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::byteFacetConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return ShortType::setMinInclusive(schema, minInclFacet);
}

void ByteType::dump(int) const
{
    std::cerr << NOTR("ByteType") << std::endl;
}

AtomicType::Type ByteType::type() const
{
    return AtomicType::BYTE;
}

CLONE_ATOMIC_TYPE(ByteType)

/*****************************************************************************
 * NonPositiveIntegerType                                                    *
 *****************************************************************************/

NonPositiveIntegerType::NonPositiveIntegerType(const Origin& origin,
                                               const NcnCred& cred)
    : IntegerType(origin, cred)
{
    String max = "0";
    setMaxInclusive(0, new MaxInclusiveFacet(max));
}

bool NonPositiveIntegerType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    int val = maxInclFacet->value().toInt();
    if (0 < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::nonPositiveFacetConstrain
                              << Message::L_ERROR  << XS_ORIGIN;
        }
        return false;
    }
    return IntegerType::setMaxInclusive(schema, maxInclFacet);
}


void NonPositiveIntegerType::dump(int) const
{
    std::cerr << NOTR("NonPositiveIntegerType") << std::endl;
}

AtomicType::Type NonPositiveIntegerType::type() const
{
    return AtomicType::NONPOSITIVEINTEGER;
}

CLONE_ATOMIC_TYPE(NonPositiveIntegerType)

/*****************************************************************************
 * NonNegativeIntegerType                                                    *
 *****************************************************************************/

NonNegativeIntegerType::NonNegativeIntegerType(const Origin& origin,
                                               const NcnCred& cred)
    : IntegerType(origin, cred)
{
    String min = "0";
    setMinInclusive(0, new MinInclusiveFacet(min));
}


bool NonNegativeIntegerType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    int val = minInclFacet->value().toInt();
    if (0 > val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::nonNegativeFacetConstrain
                              << Message::L_ERROR  << XS_ORIGIN;
        }
        return false;
    }
    return IntegerType::setMinInclusive(schema, minInclFacet);
}


void NonNegativeIntegerType::dump(int) const
{
    std::cerr << NOTR("NonNegativeIntegerType") << std::endl;
}

AtomicType::Type NonNegativeIntegerType::type() const
{
    return AtomicType::NONNEGATIVEINTEGER;
}

CLONE_ATOMIC_TYPE(NonNegativeIntegerType)

/*****************************************************************************
 * PositiveIntegerType                                                       *
 *****************************************************************************/

PositiveIntegerType::PositiveIntegerType(const Origin& origin,
                                         const NcnCred& cred)
    : NonNegativeIntegerType(origin, cred)
{
    String min = "1";
    setMinInclusive(0, new MinInclusiveFacet(min));
}

bool PositiveIntegerType::setMinInclusive(Schema* schema, MinInclusiveFacet* minInclFacet)
{
    int val = minInclFacet->value().toInt();
    if (1 > val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::positiveFacetConstrain
                              << Message::L_ERROR  << XS_ORIGIN;
        }
        return false;
    }
    return NonNegativeIntegerType::setMinInclusive(schema, minInclFacet);
}


void PositiveIntegerType::dump(int) const
{
    std::cerr << NOTR("PositiveIntegerType") << std::endl;
}

AtomicType::Type PositiveIntegerType::type() const
{
    return AtomicType::POSITIVEINTEGER;
}

CLONE_ATOMIC_TYPE(PositiveIntegerType)

/*****************************************************************************
 * NegativeIntegerType                                                       *
 *****************************************************************************/

NegativeIntegerType::NegativeIntegerType(const Origin& origin,
                                         const NcnCred& cred)
    : NonPositiveIntegerType(origin, cred)
{
    String max = NOTR("-1");
    setMaxInclusive(0, new MaxInclusiveFacet(max));
}

bool NegativeIntegerType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    int val = maxInclFacet->value().toInt();
    if (-1 < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::negativeFacetConstrain
                              << Message::L_ERROR  << XS_ORIGIN;
        }
        return false;
    }
    return NonPositiveIntegerType::setMaxInclusive(schema, maxInclFacet);
}


void NegativeIntegerType::dump(int) const
{
    std::cerr << NOTR("NegativeIntegerType") << std::endl;
}

AtomicType::Type NegativeIntegerType::type() const
{
    return AtomicType::NEGATIVEINTEGER;
}

CLONE_ATOMIC_TYPE(NegativeIntegerType)

/*****************************************************************************
 * UnsignedLongType                                                          *
 *****************************************************************************/

UnsignedLongType::UnsignedLongType(const Origin& origin,
                                   const NcnCred& cred)
    : NonNegativeIntegerType(origin, cred)
{
    String max;
    max = max.setNum(ULONG_MAX);
    setMaxInclusive(0, new MaxInclusiveFacet(max));
}


bool UnsignedLongType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    ulong val = maxInclFacet->value().toULong();
    if (ULONG_MAX < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::unsignedLongConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return NonNegativeIntegerType::setMaxInclusive(schema, maxInclFacet);
}

void UnsignedLongType::dump(int) const
{
    std::cerr << NOTR("UnsignedLongType") << std::endl;
}

AtomicType::Type UnsignedLongType::type() const
{
    return AtomicType::UNSIGNEDLONG;
}

CLONE_ATOMIC_TYPE(UnsignedLongType)

/*****************************************************************************
 * UnsignedIntType                                                           *
 *****************************************************************************/

UnsignedIntType::UnsignedIntType(const Origin& origin,
                                 const NcnCred& cred)
    : UnsignedLongType(origin, cred)
{
    String max;
    max = max.setNum(UINT_MAX);
    setMaxInclusive(0, new MaxInclusiveFacet(max));
}


bool UnsignedIntType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    uint val = maxInclFacet->value().toUInt();
    if (UINT_MAX < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::unsignedIntConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return UnsignedLongType::setMaxInclusive(schema, maxInclFacet);
}

void UnsignedIntType::dump(int) const
{
    std::cerr << NOTR("UnsignedIntType") << std::endl;
}

AtomicType::Type UnsignedIntType::type() const
{
    return AtomicType::UNSIGNEDINT;
}

CLONE_ATOMIC_TYPE(UnsignedIntType)

/*****************************************************************************
 * UnsignedShortType                                                          *
 *****************************************************************************/

UnsignedShortType::UnsignedShortType(const Origin& origin,
                                     const NcnCred& cred)
    : UnsignedIntType(origin, cred)
{
    String max;
    max = max.setNum(USHRT_MAX);
    setMaxInclusive(0, new MaxInclusiveFacet(max));
}


bool UnsignedShortType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    int val = maxInclFacet->value().toUShort();
    if (USHRT_MAX < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::unsignedShortConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return UnsignedIntType::setMaxInclusive(schema, maxInclFacet);
}

void UnsignedShortType::dump(int) const
{
    std::cerr << NOTR("UnsignedShortType") << std::endl;
}

AtomicType::Type UnsignedShortType::type() const
{
    return AtomicType::UNSIGNEDSHORT;
}

CLONE_ATOMIC_TYPE(UnsignedShortType)

/*****************************************************************************
 * UnsignedByteType                                                          *
 *****************************************************************************/

UnsignedByteType::UnsignedByteType(const Origin& origin,
                                   const NcnCred& cred)
    : UnsignedShortType(origin, cred)
{
    String max = NOTR("255");
    setMaxInclusive(0, new MaxInclusiveFacet(max));
}

bool UnsignedByteType::setMaxInclusive(Schema* schema, MaxInclusiveFacet* maxInclFacet)
{
    int val = maxInclFacet->value().toShort();
    if (255 < val) {
        if (0 != schema) {
            schema->mstream() << XsMessages::unsignedByteConstrain << Message::L_ERROR << XS_ORIGIN;
        }
        return false;
    }
    return UnsignedShortType::setMaxInclusive(schema, maxInclFacet);
}

void UnsignedByteType::dump(int) const
{
    std::cerr << NOTR("UnsignedByteType") << std::endl;
}

AtomicType::Type UnsignedByteType::type() const
{
    return AtomicType::UNSIGNEDBYTE;
}

CLONE_ATOMIC_TYPE(UnsignedByteType)

/*****************************************************************************
 * AnyUriType                                                                *
 *****************************************************************************/


AnyUriType::AnyUriType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived classes
                to know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool AnyUriType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::LENGTH:
            return setLength(schema, facet->asLengthFacet());
        case XsFacet::MINLENGTH:
            return setMinLength(schema, facet->asMinLengthFacet());
        case XsFacet::MAXLENGTH:
            return setMaxLength(schema, facet->asMaxLengthFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}


bool AnyUriType::validate(Schema* schema, const GroveLib::Node* o, const String& source,
                                String& result) const
{
    //TODO: hex-encoding for non-ASCII characters.
    bool value_to_return = true;
    result = source.simplifyWhiteSpace();
    value_to_return &= pattern_->check(schema, result, o, this);

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain << Message::L_ERROR
                        << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    uint length = result.length();
    if (0 != lenFacet_.pointer()) {
        if (length != lenFacet_->value()) {
            //! Bool flag means: true - to cut unneccecary
            // simbols from the end, false - to add spaces to
            // the end of the string to get neccecary length.
            result = result.leftJustify(lenFacet_->value(),' ', true);
            String er;
            er.setNum(lenFacet_->value());
            if (0 != schema)
                schema->mstream() << XsMessages::lengthFacetConstrain << Message::L_ERROR
                        << er << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else {
        if (0 != minLenFacet_.pointer()) {
            if (length < minLenFacet_->value()) {
                result = result.leftJustify(minLenFacet_->value(),' ', true);
                String er;
                er.setNum(minLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::minLengthFacetConstrain << Message::L_ERROR << er
                            << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        if (0 != maxLenFacet_.pointer()) {
            if (length > maxLenFacet_->value()) {
                result = result.leftJustify(maxLenFacet_->value(),' ', true);
                String er;
                er.setNum(maxLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::maxLengthFacetConstrain << Message::L_ERROR
                            << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
    }

    return value_to_return;
}

void AnyUriType::dump(int) const
{
    std::cerr << NOTR("AnyUriType") << std::endl;
}

AtomicType::Type AnyUriType::type() const
{
    return AtomicType::ANYURI;
}

bool AnyUriType::setLength(Schema* schema, LengthFacet* lenFacet)
{
    if(0 != lenFacet_.pointer()){
        if(lenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() != lenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("length") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if ((0 == minLenFacet_.pointer()) && (0 == maxLenFacet_.pointer())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else if (0 == minLenFacet_.pointer()) {
        if (maxLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("length") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    else if (0 == maxLenFacet_.pointer()) {
        if (minLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("length") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    else if ((maxLenFacet_->value() == lenFacet->value()) &&
             (minLenFacet_->value() == lenFacet->value())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                    << NOTR("length") << NOTR("minLength and maxLength") << XS_ORIGIN;
        return false;
    }
}

bool AnyUriType::setMinLength(Schema* schema, MinLengthFacet* minLenFacet)
{
    if(0 != minLenFacet_.pointer()){
        if(minLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() < minLenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("minLength") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("minLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("minLength") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    minLenFacet_ = minLenFacet;
    return true;
}

bool AnyUriType::setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet)
{
    if(0 != maxLenFacet_.pointer()){
        if(maxLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() > maxLenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("maxLength") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != maxLenFacet->value()){
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("maxLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > maxLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("maxLength") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    maxLenFacet_ = maxLenFacet;
    return true;
}

bool AnyUriType::addPattern(Schema* schema, PatternFacet* pattern)
{
    (void) schema;
    //TODO:: check for valid pattern.
    pattern_->insert(pattern);
    return true;
}

bool AnyUriType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    uint len = test.length();
    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("enumeration") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("enumeration") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                        << NOTR("enumeration") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    enumeration_->insert(enumeration);
    return true;
}

bool AnyUriType::setWhiteSpace(Schema* schema, WhiteSpaceFacet*)
{
    if (0 != schema)
        schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
    return false;
}

CLONE_ATOMIC_TYPE(AnyUriType)

/*****************************************************************************
 * AnyType                                                                   *
 *****************************************************************************/


AnyType::AnyType(const Origin& origin, const NcnCred& cred)
    : AtomicType(origin, cred)
{
    /*! TRICKY: To avoid memory leak, if exception occurs in Container's
                construction. Alternative: have OwnerPtrs in class declaration
                instead of raw pointers. But this requires derived class to
                know about FacetContainer implementation. It can lead to
                unnecessary recompilation if FacetContainer would ever be
                changed.
    */


    setWhiteSpace(0, new WhiteSpaceFacet(WhiteSpaceFacet::PRESERVE));
    pattern_ = new PatternContainer();
    enumeration_ = new EnumerationContainer();
}

bool AnyType::addFacet(Schema* schema, XsFacet* facet)
{
    switch (facet->type()) {
        case XsFacet::LENGTH:
            return setLength(schema, facet->asLengthFacet());
        case XsFacet::MINLENGTH:
            return setMinLength(schema, facet->asMinLengthFacet());
        case XsFacet::MAXLENGTH:
            return setMaxLength(schema, facet->asMaxLengthFacet());
        case XsFacet::PATTERN:
            return addPattern(schema, facet->asPatternFacet());
        case XsFacet::ENUMERATION:
            return addExtEnum(schema, facet->asEnumerationFacet());
        case XsFacet::WHITESPACE:
            return setWhiteSpace(schema, facet->asWhiteSpaceFacet());
        default:
            if (0 != schema)
                schema->mstream() << XsMessages::notRightFacet << Message::L_ERROR << XS_ORIGIN;
            return false;
    }
}

void AnyType::whiteSpacePrepare(const String& source, String& result) const
{
    if (WhiteSpaceFacet::REPLACE == whiteSpaceFacet_->value()) {
        result = source;
        String s = NOTR("[\t\n\r]"); //'[' + QChar(0x9) + QChar(0xA) + QChar(0xD) + ']';
        result.replace( QRegExp(s), " ");
    } else if (WhiteSpaceFacet::COLLAPSE == whiteSpaceFacet_->value()) {
        result = source.simplifyWhiteSpace();
    } else
        result = source;
}

bool AnyType::validate(Schema* schema,
                          const GroveLib::Node* o,
                          const String& source,
                          String& result) const
{
    bool value_to_return = true;
    whiteSpacePrepare(source, result);
    value_to_return &= pattern_->check(schema, result, o, this);

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        bool isMatched = false;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            if ( (*i)->value() == result)
                isMatched = true;
        }
        if (!isMatched) {
            if (0 != schema)
                schema->mstream() << XsMessages::enumerationFacetConstrain << Message::L_ERROR << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }

    uint length = result.length();
    if (0 != lenFacet_.pointer()) {
        if (length != lenFacet_->value()) {
            //! Bool flag means: true - to cut unneccecary
            // simbols from the end, false - to add spaces to
            // the end of the string to get neccecary length.
            result = result.leftJustify(lenFacet_->value(),' ', true);
            String er;
            er.setNum(lenFacet_->value());
            if (0 != schema)
                schema->mstream() << XsMessages::lengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
            value_to_return = false;
        }
    }
    else {
        if (0 != minLenFacet_.pointer()) {
            if (length < minLenFacet_->value()) {
                result = result.leftJustify(minLenFacet_->value(),' ', true);
                String er;
                er.setNum(minLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::minLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
        if (0 != maxLenFacet_.pointer()) {
            if (length > maxLenFacet_->value()) {
                result = result.leftJustify(maxLenFacet_->value(),' ', true);
                String er;
                er.setNum(maxLenFacet_->value());
                if (0 != schema)
                    schema->mstream() << XsMessages::maxLengthFacetConstrain << Message::L_ERROR << er << result << DV_ORIGIN(o);
                value_to_return = false;
            }
        }
    }

    return value_to_return;
}

void AnyType::dump(int indent) const
{
    (void) indent;
    std::cerr << NOTR("AnyType value")  << std::endl;
}

AtomicType::Type AnyType::type() const
{
    return AtomicType::ANYTYPE;
}

bool AnyType::setLength(Schema* schema, LengthFacet* lenFacet)
{
    if(0 != lenFacet_.pointer()){
        if(lenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() != lenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("length") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if ((0 == minLenFacet_.pointer()) && (0 == maxLenFacet_.pointer())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else if (0 == minLenFacet_.pointer()) {
        if (maxLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    else if (0 == maxLenFacet_.pointer()) {
        if (minLenFacet_->value() == lenFacet->value()) {
            lenFacet_ = lenFacet;
            return true;
        }
        else {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    else if ((maxLenFacet_->value() == lenFacet->value()) &&
             (minLenFacet_->value() == lenFacet->value())) {
        lenFacet_ = lenFacet;
        return true;
    }
    else {
        if (0 != schema)
            schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("length") << NOTR("minLength and maxLength") << XS_ORIGIN;
        return false;
    }
}

bool AnyType::setMinLength(Schema* schema, MinLengthFacet* minLenFacet)
{
    if(0 != minLenFacet_.pointer()){
        if(minLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() < minLenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("minLength") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < minLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("minLength") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    minLenFacet_ = minLenFacet;
    return true;
}

bool AnyType::setMaxLength(Schema* schema, MaxLengthFacet* maxLenFacet)
{
    if(0 != maxLenFacet_.pointer()){
        if(maxLenFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }

    if (0 < enumeration_->size()) {
        EnumerationContainer::const_iterator i;
        for(i = enumeration_->begin(); i != enumeration_->end(); i++) {
            String en = (*i)->value();
            if (en.length() > maxLenFacet->value()) {
                if (0 != schema)
                    schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR
                            <<  NOTR("maxLength") << NOTR("enumeration") << XS_ORIGIN;
                return false;
            }
        }
    }

    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != maxLenFacet->value()){
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > maxLenFacet->value()) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("maxLength") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }

    maxLenFacet_ = maxLenFacet;
    return true;
}

bool AnyType::addPattern(Schema* schema, PatternFacet* pattern)
{
    (void) schema;
    //TODO:: check for valid pattern.
    pattern_->insert(pattern);
    return true;
}

bool AnyType::addEnumeration(Schema* schema, EnumerationFacet* enumeration)
{
    String test = enumeration->value();
    uint len = test.length();
    if(0 != lenFacet_.pointer()) {
        if (lenFacet_->value() != len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("length") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != minLenFacet_.pointer()) {
        if (minLenFacet_->value() > len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("minLength") << XS_ORIGIN;
            return false;
        }
    }
    if(0 != maxLenFacet_.pointer()) {
        if (maxLenFacet_->value() < len) {
            if (0 != schema)
                schema->mstream() << XsMessages::facetsConflict << Message::L_ERROR << NOTR("enumeration") << NOTR("maxLength") << XS_ORIGIN;
            return false;
        }
    }
    enumeration_->insert(enumeration);
    return true;
}

bool AnyType::setWhiteSpace(Schema* schema, WhiteSpaceFacet* whiteSpaceFacet)
{
    if (0 != whiteSpaceFacet_.pointer()) {
        if (whiteSpaceFacet_->fixed()) {
            if (0 != schema)
                schema->mstream() << XsMessages::fixedFacet << Message::L_ERROR << NOTR("whiteSpace") << XS_ORIGIN;
            return false;
        }
    }
    whiteSpaceFacet_ = whiteSpaceFacet;
    return true;
}

CLONE_ATOMIC_TYPE(AnyType)

XS_NAMESPACE_END
