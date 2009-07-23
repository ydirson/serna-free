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

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "common/common_defs.h"
#include "common/PackedPtrSet.h"
#include "common/SubAllocator.h"
//#include "grove/grove_trace.h"
#include "common/asserts.h"
#include "common/Debug.h"

namespace Common {

void PackedVoidPointerSet::prealloc(void* p)
{
    v_ = (void**) SubAllocator<>::allocate(2 * sizeof(void*));
    v_[0] = p; v_[1] = 0;
}

void PackedVoidPointerSet::expand_alloc(void* p, void** vp)
{
    int offs = (vp - v_);
    int newsize = (1 + offs) * 2;
    void** newv = (void**) SubAllocator<>::allocate(newsize * sizeof(void*));
    memcpy(newv, v_, offs * sizeof(void*));
    SubAllocator<>::deallocate(v_, (offs + 1) * sizeof(void*));
    v_ = newv;
    vp = v_ + offs;
    *vp++ = p;
    void** last = &v_[newsize - 1];
    while (vp < last)
        *vp++ = (void*)01;
    *vp++ = 0;          // terminating 0
}

void PackedVoidPointerSet::clear()
{
    if (v_ == 0)
        return;
    void** vp = v_;
    for (; *vp; ++vp)
        ;
    SubAllocator<>::deallocate(v_, ((vp - v_) + 1) * sizeof(void*));
    v_ = 0;
}

void PackedVoidPointerSet::dump() const
{
/*
#ifdef _DEBUG
    DDBG << "PSET:";
    if (0 == v_) {
        DDBG << " <EMPTY>" << std::endl;
        return;
    }
    for (void* const* vp = v_; *vp; ++vp)
        DDBG << ' ' <<  *vp;
    DDBG << std::endl;
#endif
*/
}

}

