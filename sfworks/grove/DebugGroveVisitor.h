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

#ifndef DEBUG_GROVE_VISITOR_H_
#define DEBUG_GROVE_VISITOR_H_

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "common/RefCntPtr.h"
#include "grove/GroveVisitor.h"

GROVE_NAMESPACE_BEGIN

/*! This class designed solely for debugging purposes, and prints
 *  node change notifications on std::cerr. User can select particular
 *  notifications interesting for her by using set() and reset()
 *  member functions. These functions accept notification mask,
 *  whose possible values defined in nmask enum (they can be OR'ed).
 *
 *  If user wishes verbose output (when printed not only node name,
 *  but whole node contents), she must call verbose() member
 *  function with a true argument.
 */
class GROVE_EXPIMP DebugGroveVisitor : public GroveVisitor {
public:
    void            set(int);
    void            reset(int);
    void            verbose(bool);

    virtual void    genericNotify(const Node* node, void*);
    virtual void    nodeDestroyed(const Node* node);
    virtual void    childInserted(const Node* node);
    virtual void    childRemoved (const Node* node,
                                  const Node* child);

    virtual void    attributeChanged(const Attr* old);
    virtual void    attributeRemoved(const Element* elem, const Attr* node);
    virtual void    attributeAdded(const Attr* node);
    virtual void    textChanged(const Text* node);

    DebugGroveVisitor(GroveSectionRoot* g)
        : GroveVisitor(g), nmask_(0), verbose_(0) {}

    GROVE_OALLOC(DebugGV);

private:
    DebugGroveVisitor();
    DebugGroveVisitor(const DebugGroveVisitor&);
    DebugGroveVisitor& operator=(const DebugGroveVisitor&);

    ulong           nmask_;         // notification mask
    bool            verbose_;       // verbose output
};

inline void DebugGroveVisitor::set(int n)
{
    nmask_ |= n;
}

inline void DebugGroveVisitor::reset(int n)
{
    nmask_ &= ~n;
}

inline void DebugGroveVisitor::verbose(bool v)
{
    verbose_ = v;
}

GROVE_NAMESPACE_END

#endif // DEBUG_GROVE_VISITOR_H_
