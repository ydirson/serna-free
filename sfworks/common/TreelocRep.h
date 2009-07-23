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

#ifndef TREELOC_REP_H_
#define TREELOC_REP_H_

#include "common/common_types.h"
#include "common/asserts.h"
#include "common/Vector.h"
#include "common/StringDecl.h"

COMMON_NS_BEGIN

/*! HyTime (XPointer :) - style treeloc representation.
 */
class COMMON_EXPIMP TreelocRep {
public:

    TreelocRep() {}
    TreelocRep(const TreelocRep& tloc)
        : treeloc_(tloc.treeloc_) {}
    TreelocRep(uint reservedSize) { treeloc_.reserve(reservedSize); }

    /*! treeloc length (tree depth)
     */
    ulong   length() const { return treeloc_.size(); }
    void    clear() { treeloc_.clear(); }
    void    resize(ulong n) { treeloc_.resize(n); }
    void    append(ulong n) {
        if (treeloc_.size())
            treeloc_.insert(&treeloc_[0], n);
        else
            treeloc_.push_back(n);
    }
    /*! Return treeloc representation as a human-readable string
        (sequence of decimal node positions)
     */
    String  toString() const;

    /// Parse treeloc from string which contains sequence of decimal
    /// node positions
    bool    fromString(const String&);

    /*! Compare treelocs.
     */
    bool operator<(const TreelocRep& t) const
    {
        ulong maxlen = (t.treeloc_.size() < treeloc_.size()) ?
            t.treeloc_.size() : treeloc_.size();
        ulong i;
        for (i = 0; i < maxlen; ++i) {
            if ((*this)[i] < t[i])
                return true;
            else if ((*this)[i] > t[i])
                return false;
        }
        if (treeloc_.size() < t.treeloc_.size())
            return true;
        return false;
    }
    bool operator==(const TreelocRep& t) const
    {
        if (t.treeloc_.size() != treeloc_.size())
            return false;
        for (ulong i = 0; i < treeloc_.size(); ++i) {
            if (treeloc_[i] != t.treeloc_[i])
                return false;
        }
        return true;
    }
    bool operator!=(const TreelocRep& t) const { return !operator==(t); }

    TreelocRep& operator+=(ulong v) { treeloc_.push_back(v); return *this; }
    TreelocRep& operator+=(const TreelocRep& t)
    {
        treeloc_ += t.treeloc_; return *this;
    }
    ulong operator[](int i) const { return treeloc_[treeloc_.size() - 1 - i]; }

private:
    Vector<ulong> treeloc_;
};

COMMON_NS_END

#endif // TREELOC_REP_H_
