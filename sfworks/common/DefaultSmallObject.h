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
    Implementation of DefaultSmallObject - a SmallObject with default parameters
 */
#ifndef DEFAULT_SMALL_OBJECT_H_
#define DEFAULT_SMALL_OBJECT_H_

#include "common/common_defs.h"
#include <new>


//COMMON_NS_BEGIN
namespace COMMON_NS {
/*! Version of SmallObject<>, which does not require lenghty
    #include of SmallObject.h, but allows only default parameters.
    One constraint is that DefaultSmallObject.i must be compiled
    with the application (preferably by #including it to some
    source file).
 */
class COMMON_EXPIMP DefaultSmallObject {
public:
    static void* operator new(size_t size);
    static void operator delete(void* p, size_t size);
protected:
    ~DefaultSmallObject() {}
};

/*! You should use this macro when you work with non-polymorphic
 *  classes. This is to avoid possible overhead due to missing
 *  empty base optimization.
 */
#define USE_SMALLOBJECT_ALLOCATOR(A) \
    static void* operator new(size_t size) { \
        return A::operator new(size); \
    } \
    static void operator delete(void* p, size_t size) { \
        A::operator delete(p, size); \
    }

/*! A helper class which uses standard ::std/::new for use with
    USE_SMALL_OBJECT_ALLOCATOR macro.
 */
class COMMON_EXPIMP DefaultObjectAllocator {
public:
    static void* operator new(size_t size) {
        return ::operator new(size);
    }
    static void operator delete(void* p, size_t) {
        ::operator delete(p);
        return;
    }
};

};
//COMMON_NS_END

#endif // DEFAULT_SMALL_OBJECT_H_
