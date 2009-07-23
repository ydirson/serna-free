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
#ifndef GROVE_XINCLUDE_H_
#define GROVE_XINCLUDE_H_

#include "grove/grove_defs.h"

namespace Common {
    template <class T> class Vector;
    class String;
}

namespace GroveLib {

class Element;
class EntityReferenceStart;
class Grove;
class Node;
class Text;

// note: 'xers' means entity reference start corresponding to the section
// in the including document
GROVE_EXPIMP bool   is_xinclude_element(const Element*);
GROVE_EXPIMP void   process_xinclude_elements(const Common::Vector<Element*>&);
GROVE_EXPIMP void   xinclude_fixup_ref(const EntityReferenceStart* xers);
GROVE_EXPIMP EntityReferenceStart* xinclude_make_xers(Element*);
GROVE_EXPIMP Node*  xptr_element(Grove*, const Common::String& expr);
GROVE_EXPIMP Grove* xinclude_find_grove(EntityReferenceStart*);
GROVE_EXPIMP Grove* find_grove(Grove* g, const Common::String& path);
GROVE_EXPIMP void   sync_with_xinclude_sect(
    GroveLib::EntityReferenceStart* src,
    GroveLib::EntityReferenceStart* dst);
GROVE_EXPIMP bool   can_remove_xinclude_section(const Node* boundary);
GROVE_EXPIMP bool   xinclude_check_recursion(const EntityReferenceStart* ers,
                                             const Node* targetNode);
GROVE_EXPIMP Text*  get_grove_text(const Grove* g);
GROVE_EXPIMP void   set_lock_status(Grove* g);
GROVE_EXPIMP bool   check_has_entity(const Grove* g, const Common::String&);
GROVE_EXPIMP void   release_locks(Grove* g);

} // namespace GroveLib

#endif // GROVE_XINCLUDE_H_
