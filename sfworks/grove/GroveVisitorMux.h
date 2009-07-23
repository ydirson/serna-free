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

#ifndef GROVE_VISITOR_MUX_
#define GROVE_VISITOR_MUX_

#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "common/RefCntPtr.h"
#include "common/RadixTree.h"
#include "grove/GroveVisitor.h"

GROVE_NAMESPACE_BEGIN

/*! GroveVisitorMux used for filtering out only events that originate
 *  from some subtree represented by subtree root node, and then distributes
 *  filtered events to sub-groveVisitors.
 *  GroveVisitorMux object instance must be registered itself as a
 *  GroveVisitor in participating grove.
 */
class GROVE_EXPIMP GroveVisitorMux : public GroveVisitor {
public:
    /// A mask tells which kinds of events is interesting to our GroveVisitor's.
    enum Mask {
        node_destroyed = 00001, child_inserted = 00002,
        child_removed  = 00004, generic_notify = 00010,
        attribute_changed = 00100,  attribute_removed = 00200,
        attribute_added   = 00400, text_changed = 01000
    };
    /*! Register a sub-grove-visitor \a visitor, and filter out only
     * those events which originate below node \a root inclusive.
     * Please note that you will receive nodeDestroyed() event only
     * when you destroy root node itself - because when nodeDestroyed()
     * is called, it already doesn't have parent node, so GroveVisitorMux
     * is unable to verify whether this node is below \a root or not.
     */
    void    registerVisitor(const Node* root, GroveVisitorBase* visitor);

    /*! De-register a visitor. Note that automatic deregistering of a
     *  grove visitors on grove visitor destruction doesn't work for
     *  GroveVisitorMux, because it isn't itself a grove.
     */
    void    deregisterVisitor(const Node*);
    void    deregisterVisitor(const GroveVisitorBase*);

    /// Returns current event filter mask (described in enum Mask).
    int     mask() const { return mask_; }

    /*! Sets filter for event filtering. Only events which mentioned
     *  in mask will be passed to sub-grove-visitors.
     *  By default, ALL events are sent.
     */
    void    setMask(int mask) { mask_ = mask; }

    GroveVisitorMux(GroveSectionRoot* g)
        : GroveVisitor(g), mask_(~0) {}

    GROVE_OALLOC(GVMux);

private:
    virtual void    nodeDestroyed(const Node* node);
    virtual void    childInserted(const Node* node);
    virtual void    childRemoved (const Node* node,
                                  const Node* child);
    virtual void    textChanged(const Text* t);

    // Element only
    virtual void    genericNotify(const Node* node, void*);
    virtual void    attributeChanged(const Attr* node);
    virtual void    attributeRemoved(const Element* elem, const Attr* node);
    virtual void    attributeAdded(const Attr* node);

    static int32    phash(const Node* p);
    GroveVisitor*   getv(const Node* n);

    struct gvListItem {
        GroveVisitorBase*   gv_;
        const Node*         n_;
        gvListItem(GroveVisitorBase* gv, const Node* n);
    };
#ifdef _MSC_VER
        enum {
                htoff = 6,
                htsz  = (1 << htoff),
                hmask = ~(~0 << htoff)
        };
#else
    static const int    htoff = 6;
    static const int    htsz  = (1 << htoff);
    static const int    hmask = ~(~0 << htoff);
#endif
    COMMON_NS::RadixTree<gvListItem> gvtab_;
    int mask_;
};

inline int32 GroveVisitorMux::phash(const Node* p)
{
    return (((int32)p >> 6) & hmask);
}

inline GroveVisitorMux::gvListItem::gvListItem(GroveVisitorBase* gv,
                           const Node* n)
    : gv_(gv), n_(n)
{
}

GROVE_NAMESPACE_END

#endif // GROVE_VISITOR_MUX_
