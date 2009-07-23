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
#ifndef SAPI_GROVE_EDITOR_H_
#define SAPI_GROVE_EDITOR_H_

#include "sapi/sapi_defs.h"
#include "sapi/grove/GroveNodes.h"
#include "sapi/grove/GrovePos.h"
#include "sapi/grove/GroveStripInfo.h"
#include "sapi/common/Command.h"
#include "sapi/common/SString.h"
#include "sapi/common/PropertyNode.h"

namespace SernaApi {

/// Interface for performing operations on the XML grove instance
class SAPI_EXPIMP GroveEditor : public SimpleWrappedObject {
public:
    GroveEditor(SernaApiBase* = 0);

    /// Inserts text string at given position. If position is not within
    /// existing text, new GroveText node is created.
    Command     insertText(const GrovePos& pos, const SString& text);

    /// Removes \a count characters at current (text) position
    Command     removeText(const GrovePos& pos, unsigned int count);

    /// Replaces \a count characters at position \a pos with 
    /// \a newText (\a pos should be text position)
    Command     replaceText(const GrovePos& pos, unsigned int count,
                            const SString& newText);

    //////////////////////////////////////////////////////////////////////

    /// Insert element with specified list of attributes at the current
    /// position.
    Command     insertElement(const GrovePos& pos,
                              const SString& elementName,
                              const PropertyNode& attrs = PropertyNode());

    /// Remove specified node from the document tree.
    Command     removeNode(const GroveNode& node);

    /// Split element into two
    Command     splitElement(const GrovePos& pos);

    /// Join two neighbour elements. Both elements must be with the
    /// same name.
    Command     joinElements(const GroveElement& firstElement);

    /// Rename element at the specified position.
    Command     renameElement(const GrovePos& pos,
                              const SString& newElementName);

    //////////////////////////////////////////////////////////////////////

    /// Set new attribute value
    Command     setAttribute(const GroveAttr& attr,
                             const SString& newValue);

    /// Add new attribute to the element
    Command     addAttribute(const GroveElement& element,
                             const PropertyNode& attrSpec = PropertyNode());

    /// Remove attribute
    Command     removeAttribute(const GroveAttr& attr);

    /// Rename attribute (and change it's specs)
    Command     renameAttribute(const GroveAttr& attr,
                                const PropertyNode& attrSpec = PropertyNode());

    /// Add XMLNS mapping. Mapping to null uri means unmapping
    Command     mapXmlNs(const GroveElement& element,
                         const SString& prefix,
                         const SString& uri);

    //////////////////////////////////////////////////////////////////////

    /// Inserts entity reference to the given position.
    Command     insertEntity(const GrovePos& pos, const SString& entityName);

    //////////////////////////////////////////////////////////////////////

    /// Cut between specified positions
    Command     cut(const GrovePos& from, const GrovePos& to);

    /// Copy from between specified positions into the document fragment
    void        copy(const GrovePos& from, const GrovePos& to,
                     const GroveDocumentFragment& clipboard);

    /// Paste document fragment to the specified position
    Command     paste(const GroveDocumentFragment& clipboard,
                      const GrovePos& to);

    //////////////////////////////////////////////////////////////////////

    /// Wrap specified range into new element
    Command     tagRegion(const GrovePos& from,
                          const GrovePos& to,
                          const SString& newElemName,
                          const PropertyNode& attrMap = PropertyNode());

    /// Unwrap element at the specified position
    Command     untag(const GrovePos& pos);

    //////////////////////////////////////////////////////////////////////
    /// Inserts PI element at given position.
    Command     insertPi(const GrovePos& pos,
                         const SString& target,
                         const SString& text);
    /// Changes PI element at given position.
    Command     changePi(const GrovePos& pos,
                         const SString& target,
                         const SString& newtext);
    /// Inserts comment element at given position.
    Command     insertComment(const GrovePos& pos,
                              const SString& text);
    /// Changes comment element at given position.
    Command     changeComment(const GrovePos& pos,
                              const SString& newText);
    
    /// Turns selected text into the redline
    Command     convertToRedline(const GrovePos& first,
                                 const GrovePos& last,
                                 unsigned int redlineMask,
                                 const SString& annotation);
    
    /// Expands current redline
    Command     convertFromRedline(const GrovePos& pos);
    
    /// Change properties of the current redline
    Command     changeRedline(const GrovePos& pos,
                              unsigned int redlineMask,
                              const SString& annotation);

    /// Insert new (empty) redline
    Command     insertRedline(const GrovePos& pos,
                              unsigned int redlineMask,
                              const SString& annotation);
    /// Insert new XInclude to \a pos.
    /// Property tree may have the following properties on the top level:
    /// - href (required)      - the URL for the inclusion
    /// - xpointer (optional)  - ID for inclusion 
    /// - encoding (optional)  - encoding
    /// - parse (optional, default=xml) - parse type
    /// - attrs                - additional optional attributes. The children
    /// of 'attrs' must have name corresponding to the attribute name, value
    /// corresponding to the attribute URI, and child property 'value' which
    /// must hold the attribute value.
    Command     insertXinclude(const GrovePos& pos,
                               const PropertyNode& props,
                               const GroveStripInfo& si = GroveStripInfo(), 
                               bool isText = false);

    /// Declare new entity or notation. This method does not create
    /// any entity references.
    /// Property tree may take the following properties on the top level:
    /// - name  - entity/notation name
    /// - decl-type - entity declaration type, may be (internal,
    ///   external, notation)
    /// - data-type - data type (cdata, ndata, sgml)
    /// - content - internal entity content or sysid
    /// - notation - notation name, meaningful for external entities only
    Command     declareEntity(const GrovePos& pos,
                              const PropertyNode& props);

    /// Returns error message
    SString     errorMessage() const;

private:
    SString     errorMessage_;
};

} // namespace SernaApi

#endif // SAPI_GROVE_EDITOR_H_

