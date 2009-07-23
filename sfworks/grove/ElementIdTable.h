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

#ifndef ELEMENT_ID_TABLE_H_
#define ELEMENT_ID_TABLE_H_

#include "common/PtrSet.h"
#include "grove/grove_exports.h"
#include "grove/grove_defs.h"
#include "grove/Nodes.h"
#include "grove/XNodePtr.h"

GROVE_NAMESPACE_BEGIN

/*! This is the place where the grove keeps ID attribute references.
 *  There is two models for ID attribute processing in grove: in the
 *  default mode, changes to the table made automatically when you
 *  add, delete or change ID attribute of some element; however,
 *  as with any attribute, you must call element->notifyChange(oldAttr)
 *  if you change some attribute by reference, not via corresponding Element
 *  member functions. In the second mode, automatic processing of ID
 *  attributes in grove is disabled by specifying GroveBuilder::dontProcessIds
 *  flag for the grove builder, so you must make all changes to ID
 *  table manually.
 *
 *  If you need to iterate ID table, table() and constTable() accessors
 *  should be used for constructing normal and const iterators,
 *  respectively. An example:
 *  \code
 *    ElementIdTable::constAttrTableIter iter(grove->idTable()->constTable());
 *    const Attr* a;
 *    while ((a = iter.next.pointer()))
 *  do_something(a);
 *  \endcode
 */
class GROVE_EXPIMP ElementIdTable {
public:
    typedef Common::String String;

    template <class V, class K> class AttrKeyFunction {
    public:
        typedef K KeyType;
        static const K& key(const COMMON_NS::PtrBase<V>& value)
                           { return value->value(); }
        static const K& key(const V* value) { return value->value(); }
    };

    typedef COMMON_NS::SmartPtrSet<COMMON_NS::RefCntPtr<Attr>,
                     AttrKeyFunction<Attr, COMMON_NS::String> > attrTable;
    typedef attrTable::iterator iterator;
    typedef attrTable::const_iterator const_iterator;

    /// Check the presense of \a idstring in the table
    bool            hasId(const String& idstring) const
    {
        return (enabled_ && t_.end() != t_.find(idstring));
    }
    /// Get pointer to the element with ID equal to given \a idstring
    Element*        lookupElement(const String& idstring) const
    {
        if (!enabled_)
            return 0;
        const Attr* a = lookupAttr(idstring);
        if (0 == a || 0 == a->parent())
            return 0;
        return a->element();
    }
    /// Get pointer to the attribute node which contain \a idstring
    Attr*           lookupAttr(const String& idstring) const
    {
        if (!enabled_)
            return 0;
        iterator it = t_.find(idstring);
        if (t_.end() == it)
            return 0;
        return it->pointer();
    }
    /// Add/replace new ID attribute node
    void            addReplace(Attr* idattr)
    {
        if (!enabled_)
            return;
        t_.insert(idattr);
    }
    /// Remove #ID from table
    void            removeId(const String& idstring)
    {
        if (!enabled_)
            return;
        iterator it = t_.find(idstring);
        if (t_.end() != it)
            t_.erase(it);
    }
    /// Accessor for direct access to ID attribute node table.
    attrTable&       table() { return t_; }

    /*! Check whether automatic ID processing is enabled */
    bool            isProcessingEnabled() const { return enabled_; }

    /// Check ID processing status
    void            setIdProcessing(bool enable);

    void            dump() const;

    ElementIdTable(bool enabled = true)
        : enabled_(enabled) {}

private:
    attrTable       t_;
    bool            enabled_;
};

GROVE_NAMESPACE_END

#endif // ELEMENT_ID_TABLE_H_
