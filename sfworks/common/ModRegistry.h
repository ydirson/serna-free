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
#ifndef COMMON_MODREGISTRY_H_
#define COMMON_MODREGISTRY_H_

#include "common/common_defs.h"
#include "common/XTreeNode.h"
#include "common/TreelocRep.h"
#include <set>

COMMON_NS_BEGIN

/*! This is a template for frameworks which need to register modified
 *  instances in tree order. It is safe to use in cases when tree is
 *  modified in process, but tree nodes must call deregisterInstance()
 *  on their destruction.
 *  Note that tree nodes must be XTreeNode's.
 */
template <class T> class ModRegistry {
public:
    typedef std::set<T*> InstanceSet;

    void    registerInstance(T* inst)   { is_.insert(inst); }
    void    deregisterInstance(T* inst) { is_.erase(inst); }

    T*      getFirst(bool remove = false)
    {
        if (is_.empty())
            return 0;
        COMMON_NS::TreelocRep pv(128), pv1(128);
        typename InstanceSet::iterator lower = is_.begin();
        typename InstanceSet::iterator i = lower;
        ++i;
        if (i != is_.end()) {
            for (; i != is_.end(); ++i) {
                (*lower)->getTreeloc(pv);
                (*i)->getTreeloc(pv1);
                if (pv1 < pv)
                    lower = i;
            }
        }
        T* temp = *lower;
        if (remove)
            is_.erase(lower);
        return temp;
    }
    bool hasInstance(T* i) const { return is_.find(i) != is_.end(); }

    bool isEmpty() const { return is_.empty(); }

    ulong size() const { return is_.size(); }

    void clear() { is_.clear(); }

    // for debug purposes
    const InstanceSet& instanceSet() const { return is_; }

private:
    InstanceSet     is_;
};

COMMON_NS_END

#endif // COMMON_MODREGISTRY_H_
