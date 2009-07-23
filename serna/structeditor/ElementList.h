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
#ifndef STRUCTEDITOR_IMPL_ELEMENT_LIST_H_
#define STRUCTEDITOR_IMPL_ELEMENT_LIST_H_

#include "structeditor/StructEditor.h"
#include "groveeditor/GrovePos.h"
#include "common/PropertyTree.h"
#include "common/CommandEvent.h"
#include <set>

class PropertyNodeNameCmp {
public:
    bool operator()(const Common::PropertyNodePtr& n1,
                    const Common::PropertyNodePtr& n2) const
    {
        const Common::Char* cp1 = n1->name().unicode();
        const Common::Char* ce1 = cp1 + n1->name().length();
        const Common::Char* cp2 = n2->name().unicode();
        const Common::Char* ce2 = cp2 + n2->name().length();
        for (; cp1 < ce1 && *cp1 != ':'; ++cp1)
            ;
        for (; cp2 < ce2 && *cp2 != ':'; ++cp2)
            ;
        cp1 = (cp1 >= ce1) ? n1->name().unicode() : (++cp1, cp1);
        cp2 = (cp2 >= ce2) ? n2->name().unicode() : (++cp2, cp2);
        for (;; ++cp1, ++cp2) {
            if (cp1 >= ce1 || cp2 >= ce2) {
                const int l1 = ce1 - cp1;
                const int l2 = ce2 - cp2;
                if (l1 == l2)
                    break;
                return l1 < l2;
            }
            if (*cp1 < *cp2)
                return true;
            if (*cp1 > *cp2)
                return false;
        }
        if (n1->getString().isEmpty() && n2->getString().isEmpty())
            return false;
        return n1->getString() < n2->getString();
    }
};

class STRUCTEDITOR_EXPIMP ElementList :
    private std::set<Common::PropertyNodePtr, PropertyNodeNameCmp> {
public:
    typedef std::set<Common::PropertyNodePtr, PropertyNodeNameCmp> PtnSet;

    ElementList(StructEditor*);
    ~ElementList();

    // Returns true if element elemName can be inserted at the current position
    bool    canInsert(const Common::String& elemName, 
                      const Common::String& ns = "",
                      const GroveEditor::GrovePos* = 0);

    // Returns alphabetically sorted list of elements
    bool    fillElemList(Common::PropertyNode*);

    bool    getRenameSet(PtnSet&);
    void    setModified() { modified_ = true; }

protected:
    friend class InsertElementUtils;
    
    bool        reload(const GroveEditor::GrovePos* = 0);
    bool        updatePos(const GroveEditor::GrovePos* = 0);
    bool        get_list(PtnSet& to, bool rename);

    StructEditor*         se_;
    GroveEditor::GrovePos pos_;
    bool                  modified_;
    bool                  isAny_;
};

#endif // STRUCTEDITOR_IMPL_ELEMENT_LIST_H_

