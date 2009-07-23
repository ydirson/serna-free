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
    Implementation file for Singleton.h
 */
#include "common/common_defs.h"
#include "common/Singleton.h"


COMMON_NS_BEGIN
# ifdef COMMON_PRAGMA_BEGIN
# pragma COMMON_PRAGMA_BEGIN
# endif // - COMMON_PRAGMA_BEGIN

namespace SingletonPrivate {

    TrackerArray pTrackerArray = 0;
    unsigned int elements = 0;

    /*! Ensures proper destruction of objects with longevity
     */
    void AtExitFn()
    {
        using namespace std;
        assert(elements > 0 && pTrackerArray != 0);
        // Pick the element at the top of the stack
        LifetimeTracker* pTop = pTrackerArray[elements - 1];
        // Remove that object off the stack
        // Don't check errors - realloc with less memory
        //     can't fail
        pTrackerArray = static_cast<TrackerArray>
            (realloc(pTrackerArray, --elements));
        // Destroy the element
        delete pTop;
    }
} // namespace SingletonPrivate

# ifdef COMMON_PRAGMA_END
# pragma COMMON_PRAGMA_END
# endif // - COMMON_PRAGMA_END
COMMON_NS_END
