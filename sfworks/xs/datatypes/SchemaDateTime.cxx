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
#include "xs/datatypes/SchemaDateTime.h"

USING_COMMON_NS

XS_NAMESPACE_BEGIN

const bool SchemaDateTime::isNull() const
{
    if ((0 == sign) &&
        (0 == year) &&
        (0 == month) &&
        (0 == day) &&
        (0 == hour) &&
        (0 == minute) &&
        (0 == second))
        return true;
    return false;
}


const bool SchemaDateTime::operator == (const SchemaDateTime& value) const
{
    if ((sign == value.sign) &&
        (year == value.year) &&
        (month == value.month) &&
        (day == value.day) &&
        (hour == value.hour) &&
        (minute == value.minute) &&
        (second == value.second))
        return true;
    return false;
}

const bool SchemaDateTime::operator < (const SchemaDateTime& value) const
{
    if (sign != value.sign) {
        if (sign) {
            return false;
        }
        else
            return true;
    }

    if (year != value.year) {
        if (sign) {
            return (year < value.year);
        }
        else {
            return (value.year <  year);
        }
    }

    if (month != value.month){
        if (sign) {
            return (month < value.month);
        }
        else {
            return (value.month < month);
        }
    }

    if (day != value.day) {
        if (sign) {
            return (day < value.day);
        }
        else {
            return (value.day < day);
        }
    }

    if (hour != value.hour) {
        if (sign) {
            return (hour < value.hour);
        }
        else {
            return (value.hour < hour);
        }
    }

    if (minute != value.minute) {
        if (sign) {
            return (minute < value.minute);
        }
        else {
            return (value.minute < minute);
        }
    }

    if (second != value.second) {
        if (sign) {
            return (second < value.second);
        }
        else {
            return (value.second < second);
        }
    }

    return true;
}

const bool SchemaDateTime::operator <= (const SchemaDateTime& value) const
{
    return ((this->operator < (value)) || (this->operator==(value)));
}


bool SchemaDateTime::checkConstrain(SchemaDateTime& result) const
{
    bool is_ready = true;
    if (60 <= result.second) {
        result.second = result.second - 60;
        result.minute++;
        is_ready = false;
    }
    else if (0 > result.second) {
        result.second = 60 + result.second;
        result.minute--;
        is_ready = false;
    }

    if (60 <= result.minute) {
        result.minute = result.minute - 60;
        result.hour++;
        is_ready = false;
    }
    else if (0 > result.minute) {
        result.minute = 60 + result.minute;
        result.hour--;
        is_ready = false;
    }

    if (24 <= result.hour) {
        result.hour = result.hour - 24;
        result.day++;
        is_ready = false;
    }
    else if (0 > result.hour) {
        result.hour = 60 + result.hour;
        result.day--;
        is_ready = false;
    }

    if ((28 < result.day) && (2 == result.month)) {
        if (0 == (result.year % 4)) {
            if (29 != result.day) {
                result.day = result.day - 29;
                result.month++;
                is_ready = false;
            }
        }
        else {
            result.day = result.day - 28;
            result.month++;
            is_ready = false;
        }
    }
    else if ((0 >= result.day) && (2 == result.month)) {
        if (0 == (result.year % 4)) {
            result.day = 29 + result.day;
            result.month--;
            is_ready = false;
        }
        else {
            result.day = 28 + result.day;
            result.month--;
            is_ready = false;
        }
    }

    if (30 < result.day) {
        if ((1 == result.month) ||
            (3 == result.month) ||
            (5 == result.month) ||
            (7 == result.month) ||
            (8 == result.month) ||
            (10 == result.month) ||
            (12 == result.month)) {
            if (31 != result.day) {
                result.day = result.day - 31;
                result.month++;
                is_ready = false;
            }
        }
        else {
            result.day = result.day - 30;
            result.month++;
            is_ready = false;
        }
    }
    else if (0 >= result.day) {
        if ((1 == result.month) ||
            (3 == result.month) ||
            (5 == result.month) ||
            (7 == result.month) ||
            (8 == result.month) ||
            (10 == result.month) ||
            (12 == result.month)) {
            result.day = 31 + result.day;
            result.month--;
            is_ready = false;
        }
        else {
            result.day = 30 + result.day;
            result.month--;
            is_ready = false;
        }
    }

    if (12 < result.month) {
        result.month = result.month - 12;
        result.year++;
        is_ready = false;
    }
    else if (0 >= result.month) {
        result.month = 12 + result.month;
        result.year--;
        is_ready = false;
    }

    if (!is_ready) {
        is_ready = checkConstrain(result);
    }
    else {
        if (0 > result.year) {
            result.sign = !result.sign;
            result.year = -result.year;
        }
        return is_ready;
    }

    //to be compiler happy.
    return true;
}


SchemaDateTime SchemaDateTime::operator - (const SchemaDateTime& value) const
{
    SchemaDateTime result;
    result.sign = sign;

    if ((value.sign && sign) ||
        (!value.sign && !sign)) {
        result.year   = year - value.year;
        result.month  = month - value.month;
        result.day    = day - value.day;
        result.hour   = hour - value.hour;
        result.minute = minute - value.minute;
        result.second = second - value.second;
    }
    else {
        result.year   = year + value.year;
        result.month  = month + value.month;
        result.day    = day + value.day;
        result.hour   = hour + value.hour;
        result.minute = minute + value.minute;
        result.second = second + value.second;
    }

    checkConstrain(result);
    return result;
}

SchemaDateTime SchemaDateTime::operator + (const SchemaDateTime& value) const
{
    SchemaDateTime result;
    result.sign = sign;

    if ((value.sign && sign) ||
        (!value.sign && !sign)) {
        result.year   = year + value.year;
        result.month  = month + value.month;
        result.day    = day + value.day;
        result.hour   = hour + value.hour;
        result.minute = minute + value.minute;
        result.second = second + value.second;
    }
    else {
        result.year   = year - value.year;
        result.month  = month - value.month;
        result.day    = day - value.day;
        result.hour   = hour - value.hour;
        result.minute = minute - value.minute;
        result.second = second - value.second;
    }

    checkConstrain(result);
    return result;
}

XS_NAMESPACE_END
