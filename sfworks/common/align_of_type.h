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
    Provides (compile-time) information about type alignment
 */

#ifndef ALIGN_OF_TYPE_H_
#define ALIGN_OF_TYPE_H_

#include "common/common_defs.h"


#ifdef __GNUC__
# define ALIGN_OF_TYPE(T) __alignof__(T)
# else // less beautiful

template struct AlignOfTypeHelper__<T> {
    bool    start_;
    T       end_;
    char*   pad_;
};

# define ALIGN_OF_TYPE(T) \
    ((char*)&((AlignOfTypeHelper__<T>*)0)->end_ - \
        (char*)&((AlignOfTypeHelper__<T>*)0)->start_)

#endif // __GNUC__

/**
 * Align address to boundary
 * @arg addr address
 * @arg bdry boundary
 * @return aligned address
 */
inline unsigned int align_to_boundary(unsigned int addr, unsigned int bdry)
{
    return ((addr + bdry-1) / bdry) * bdry;
}

#endif // ALIGN_OF_TYPE_H_
