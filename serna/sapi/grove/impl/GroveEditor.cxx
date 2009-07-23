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
#include "sapi/sapi_defs.h"
#include "sapi/grove/GroveCommand.h"
#include "sapi/grove/GroveEditor.h"
#include "sapi/grove/GrovePos.h"
#include "groveeditor/GroveEditor.h"
#include "groveeditor/Exception.h"
#include "grove/Node.h"
#include "common/PropertyTree.h"
#include "common/safecast.h"

#define NODE_IMPL(n) SAFE_CAST(GroveLib::Node*, n.getRep())
#define ATTR_IMPL(n) SAFE_CAST(GroveLib::Attr*, n.getRep())
#define ELEM_IMPL(n) SAFE_CAST(GroveLib::Element*, n.getRep())
#define SELF         SAFE_CAST(::GroveEditor::Editor*, getRep())
#define PTN_IMPL(p)  SAFE_CAST(Common::PropertyNode*, p.getRep())
#define SI_IMPL(p)   SAFE_CAST(GroveLib::StripInfo*, p.getRep())

#define GEDIT_TRY(f) try { if (!getRep()) return 0; \
    Command cmd = SELF->f .pointer(); \
    if (!cmd.isNull()) return cmd; \
        errorMessage_ = SELF->errorMessage(); return 0; }

#define GEDIT_CATCH catch (::GroveEditor::Exception& e) { \
    errorMessage_ = e.what(); return 0; }  return 0;

#define GEDIT_WRAP(f1, f2, chk) \
    Command GroveEditor::f1 { chk; GEDIT_TRY(f2) GEDIT_CATCH; }

#define CHECK_POS(pos) if (pos.isNull()) return 0
#define CHECK_SEL(from, to) if (from.isNull() || to.isNull()) return 0
#define CHECK_NODE(n)  if (!n.getRep()) return 0
#define CHECK_ATTR(n)  if (!n.getRep() || \
    NODE_IMPL(n)->nodeType() != GroveLib::Node::ATTRIBUTE_NODE) return 0
#define CHECK_ELEM(n)  if (!n.getRep() || \
    NODE_IMPL(n)->nodeType() != GroveLib::Node::ELEMENT_NODE) return 0

namespace SernaApi {

GroveEditor::GroveEditor(SernaApiBase* s)
    : SimpleWrappedObject(s)
{
}

GEDIT_WRAP(insertText(const GrovePos& pos, const SString& text),
    insertText(pos, text), CHECK_POS(pos))

GEDIT_WRAP(removeText(const GrovePos& pos, uint count),
    removeText(pos, count), CHECK_POS(pos))

GEDIT_WRAP(replaceText(const GrovePos& pos, uint count, const SString& t),
    replaceText(pos, count, t), CHECK_POS(pos))

GEDIT_WRAP(insertElement(const GrovePos& pos, const SString& ename,
    const PropertyNode& attrs), insertElement(pos, ename, PTN_IMPL(attrs)),
    CHECK_POS(pos))

GEDIT_WRAP(removeNode(const GroveNode& node), removeNode(NODE_IMPL(node)),
    CHECK_NODE(node))

GEDIT_WRAP(splitElement(const GrovePos& pos), splitElement(pos), 
    CHECK_POS(pos))

GEDIT_WRAP(joinElements(const GroveElement& elem),
    joinElements(ELEM_IMPL(elem)), CHECK_NODE(elem))

GEDIT_WRAP(renameElement(const GrovePos& pos, const SString& ename),
    renameElement(pos, ename), CHECK_POS(pos))

GEDIT_WRAP(setAttribute(const GroveAttr& attr, const SString& v),
    setAttribute(ATTR_IMPL(attr), v), CHECK_ATTR(attr))

GEDIT_WRAP(addAttribute(const GroveElement& elem, const PropertyNode& aspec),
    addAttribute(ELEM_IMPL(elem), PTN_IMPL(aspec)), CHECK_ELEM(elem))

GEDIT_WRAP(removeAttribute(const GroveAttr& attr),
    removeAttribute(ATTR_IMPL(attr)), CHECK_ATTR(attr))

GEDIT_WRAP(renameAttribute(const GroveAttr& attr, const PropertyNode& aspec),
    renameAttribute(ATTR_IMPL(attr), PTN_IMPL(aspec)), CHECK_ATTR(attr))

GEDIT_WRAP(mapXmlNs(const GroveElement& elem, const SString& prefix,
    const SString& uri), mapXmlNs(ELEM_IMPL(elem), prefix, uri),
    CHECK_ELEM(elem))

GEDIT_WRAP(insertEntity(const GrovePos& pos, const SString& ename),
    insertEntity(pos, ename, Common::String(), 0), CHECK_POS(pos))

GEDIT_WRAP(cut(const GrovePos& from, const GrovePos& to), 
    cut(from, to), CHECK_SEL(from, to))

void GroveEditor::copy(const GrovePos& from, const GrovePos& to,
                       const GroveDocumentFragment& clpbd)
{
    try {
        if (from.isNull() || to.isNull())
            return;
        SELF->copy(from, to,
            SAFE_CAST(GroveLib::DocumentFragment*, clpbd.getRep()));
    }
    catch (::GroveEditor::Exception& e) {
        errorMessage_ = e.what();
    }
}

GEDIT_WRAP(paste(const GroveDocumentFragment& clpbd, const GrovePos& to),
    paste(SAFE_CAST(GroveLib::DocumentFragment*, clpbd.getRep()), to),
    if (clpbd.isNull() || to.isNull()) return 0)

GEDIT_WRAP(tagRegion(const GrovePos& from, const GrovePos& to,
    const SString& ename, const PropertyNode& attrMap),
        tagRegion(from, to, ename, PTN_IMPL(attrMap)), CHECK_SEL(from, to))

GEDIT_WRAP(untag(const GrovePos& pos), untag(pos), CHECK_POS(pos))

GEDIT_WRAP(insertPi(const GrovePos& pos, const SString& target,
    const SString& text), insertPi(pos, target, text), CHECK_POS(pos))

GEDIT_WRAP(changePi(const GrovePos& pos, const SString& target,
    const SString& text), changePi(pos, target, text), CHECK_POS(pos))

GEDIT_WRAP(insertComment(const GrovePos& pos, const SString& text),
    insertComment(pos, text), CHECK_POS(pos))

GEDIT_WRAP(changeComment(const GrovePos& pos, const SString& text),
    changeComment(pos, text), CHECK_POS(pos))

GEDIT_WRAP(convertToRedline(const GrovePos& from, const GrovePos& to,
    uint redlineMask, const SString& annotation), 
    convertToRedline(from, to, redlineMask, annotation), CHECK_SEL(from, to))   

GEDIT_WRAP(convertFromRedline(const GrovePos& pos),
    convertFromRedline(pos), CHECK_POS(pos))
    
GEDIT_WRAP(changeRedline(const GrovePos& pos, uint rmask, const SString& ann),
    changeRedline(pos, rmask, ann), CHECK_POS(pos))

GEDIT_WRAP(insertRedline(const GrovePos& pos, uint rmask, const SString& ann),
    insertRedline(pos, rmask, ann), CHECK_POS(pos))

GEDIT_WRAP(insertXinclude(const GrovePos& pos, const PropertyNode& props,
    const GroveStripInfo& si, bool isText),
    insertXinclude(pos, PTN_IMPL(props), SI_IMPL(si), isText), CHECK_POS(pos))

GEDIT_WRAP(declareEntity(const GrovePos& pos, const PropertyNode& props),
    declareEntity(pos, PTN_IMPL(props)), CHECK_POS(pos))

SString GroveEditor::errorMessage() const
{
    if (getRep())
        return errorMessage_;
    return SString();
}

} // namespace SernaApi

