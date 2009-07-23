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
#ifndef GROVE_UDATA_H_
#define GROVE_UDATA_H_

#include "grove/Node.h"

namespace GroveLib {

enum {
    MODIFIED_BIT   = 0x800000,      // force XSLT instance re-make
    FOLD_BIT       = 0x400000,      // folded node
    XSLT_GEN_BIT   = 0x200000,      // XSLT-generated special node
    CM_FOLD_BIT    = 0x100000,      // contentmap tree state
    ID_SCOPE_BIT   = 0x080000,      // this node defines ID scope
    REDLINE_MASK   = 0x07FFFF       // redline ID
};

// force XSLT instance re-make

inline bool is_node_modified(const Node* n)
{
    return n->udata() & MODIFIED_BIT;
}

inline void set_node_modified(Node* n, bool v = false)
{
    if (v)
        n->setUdata(n->udata() | MODIFIED_BIT);
    else
        n->setUdata(n->udata() & ~MODIFIED_BIT);
}

/////////////////////////////////////////////////////////////////////

/// Folding
inline bool is_node_folded(const Node* n)
{
    return n->udata() & FOLD_BIT;
}

inline void set_node_fold_state(Node* n, bool v = false)
{
    if (v)
        n->setUdata(n->udata() | FOLD_BIT);
    else
        n->setUdata(n->udata() & ~FOLD_BIT);
}

/////////////////////////////////////////////////////////////////////

// ID scoping

inline bool is_id_scope_ext(const Node* n)
{
    return n->udata() & ID_SCOPE_BIT;
}

inline void set_id_scope_ext(Node* n, bool v)
{
    if (v)
        n->setUdata(n->udata() | ID_SCOPE_BIT);
    else
        n->setUdata(n->udata() & ~ID_SCOPE_BIT);
}

/////////////////////////////////////////////////////////////////////

// Redlining

inline bool has_redline(const Node* n) 
{
    return n->udata() & REDLINE_MASK;
}

inline uint redline_info(const Node* n)
{
    return n->udata() & REDLINE_MASK;
}

inline void 
set_redline_info(Node* n, int redlineId = 0)
{
    n->setUdata((n->udata() & ~REDLINE_MASK) | (redlineId & REDLINE_MASK));
}

} // namespace

#endif // GROVE_UDATA_H_
