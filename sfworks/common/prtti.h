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
    Base for all owning/reference counting smart pointers
 */
#ifndef PRTTI_H_
#define PRTTI_H_

#include "common/common_defs.h"


/**
 * Simple pseudo-RTTI framework (based on virtual functions return type
 * polymorphism feature of C++).
 *
 * Usage:
 *  - in base class, use PRTTI_DECL(subtype)
 *  - in subtype class, use PRTTI_DECL(subtype)
 *  - in base class impl. cxx file, use PRTTI_BASE_STUB(base, subtype)
 *  - in subtype class impl. cxx file, use PRTTI_IMPL(subtype)
 *  - when implementing subtype rtti not in subtype class itself, use
 *    PRTTI_IMPL2(mid-base, subtype). This is useful for avoiding cross-casts.
 */
#define PRTTI_DECL(type) \
    virtual type* as##type(); \
    virtual const type* asConst##type() const

#define PRTTI_DECL_NS(type, ns) \
    virtual ns::type* as##type(); \
    virtual const ns::type* asConst##type() const

#define PRTTI_BASE_STUB(base, type) \
    type* base::as##type() { return 0; } \
    const type* base::asConst##type() const { return 0; }

#define PRTTI_BASE_STUB_NS(base, type, ns) \
    ns::type* base::as##type() { return 0; } \
    const ns::type* base::asConst##type() const { return 0; }

#define PRTTI_IMPL(type) \
    type* type::as##type() { return this; } \
    const type* type::asConst##type() const { return this; }

#define PRTTI_IMPL2(base, type) \
    type* base::as##type() { return this; } \
    const type* base::asConst##type() const { return this; }

#define PRTTI_IMPL2_NS(base, type, ns) \
    ns::type* base::as##type() { return this; } \
    const ns::type* base::asConst##type() const { return this; }

#endif // PRTII_H_
