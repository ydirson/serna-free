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
 *
 ***********************************************************************/

#ifndef GROVE_EDITOR_H
#define GROVE_EDITOR_H

#include "groveeditor/groveeditor_defs.h"
#include "groveeditor/GroveEditorMessages.h"

#include "common/RefCntPtr.h"
#include "common/STQueue.h"
#include "common/Vector.h"
#include "common/String.h"
#include "common/Command.h"

#include "grove/Grove.h"
#include "grove/Nodes.h"
#include "grove/StripInfo.h"
#include "grove/ExternalId.h"

namespace Common {
    class PropertyNode;
}

namespace GroveEditor
{

class GrovePos;

GROVEEDITOR_EXPIMP void make_attributes(GroveLib::Element* elem,
                                        GroveLib::Grove* grove,
                                        const Common::PropertyNode* attrs);

class GROVEEDITOR_EXPIMP Editor : public SernaApiBase {
public:
    typedef Common::CommandPtr CommandPtr;
    typedef Common::RefCntPtr<GroveLib::DocumentFragment> FragmentPtr;

    Editor();
    virtual ~Editor();

// TEXT OPERATIONS
    /// Inserts text into existing text node
    CommandPtr  insertText(const GrovePos& pos,
                           const Common::String& text);

    //! Removest 'count' charactars from 'pos' of TextNode
    CommandPtr  removeText(const GrovePos& pos, uint count);

    /// Replace text
    CommandPtr  replaceText(const GrovePos& pos, uint count,
                            const Common::String& newText);

    CommandPtr  del(const GrovePos& pos);
    CommandPtr  backspace(const GrovePos& pos);

// ELEMENT OPERATIONS
    ///
    CommandPtr  insertElement(const GrovePos& pos,
                              const Common::String& elementName,
                              const Common::PropertyNode* attrs);
    //!
    CommandPtr  removeNode(const GroveLib::NodePtr& node);
    //!
    CommandPtr  splitElement(const GrovePos& pos);
    //!
    CommandPtr  joinElements(const GroveLib::NodePtr& firstNode);
    //!
    CommandPtr  renameElement(const GrovePos& pos,
                              const Common::String& elementName);
// ATTRIBUTE OPERATIONS
    CommandPtr  setAttribute(GroveLib::Attr* attr,
                             const Common::String& newValue);

    CommandPtr  addAttribute(GroveLib::Element* element,
                             const Common::PropertyNode* attrSpec);

    CommandPtr  removeAttribute(GroveLib::Attr* attr);

    // Rename actually removes attr and re-creates new attribute from attrSpec
    CommandPtr  renameAttribute(GroveLib::Attr* attr,
                                const Common::PropertyNode* attrSpec);

    // Mapping to .isNull() uri means unmapping
    CommandPtr  mapXmlNs(GroveLib::Element* element,
                         const Common::String& prefix,
                         const Common::String& uri);

//! ENTITY OPERATIONS
    CommandPtr  convertToEntity(const GrovePos& first,
                                const GrovePos& last,
                                const Common::String& name,
                                const GroveLib::ExternalId* eid = 0);
    CommandPtr  convertFromEntity(const GrovePos& pos,
                                  bool convertAll = false);

    //! Inserts entity reference. If p is specified, try to make
    //! external entity with sysid p
    CommandPtr  insertEntity(const GrovePos& pos,
                             const Common::String& entityName,
                             const Common::String& path,
                             const GroveLib::StripInfo* si);

    //! Changes declaration/removes entity
    CommandPtr  changeEntityDecl(const GrovePos& pos,
                                 const Common::String& name,
                                 const Common::String& newName,
                                 const Common::String& newPath);

    //! Declare entity or notation. Properties: name,
    //! decl-type=(internal|external|notation), 
    //! data-type=(cdata|ndata|sgml),
    //! content=<content for internal entity, sysid for external or notation>,
    //! notation=<notation-name>
    CommandPtr  declareEntity(const GrovePos& pos,
                              const Common::PropertyNode* entitySpec);

//! MIXED CONTENT OPERATIONS
    //!
    CommandPtr  cut(const GrovePos& from, const GrovePos& to,
                    GrovePos* adjustPos = 0);
    //!
    bool        copy(const GrovePos& from, const GrovePos& to,
                     const FragmentPtr& clipboard);
    //!
    CommandPtr  paste(const FragmentPtr& clipboard,
                      const GrovePos& to, int toDel = 0,
                      bool insertRef = false);

    CommandPtr  tagRegion(const GrovePos& from,
                          const GrovePos& to,
                          const Common::String& elemName,
                          const Common::PropertyNode* attrMap);

    CommandPtr  untag(const GrovePos& pos);

/// PI/COMMENT commands
    CommandPtr  insertPi(const GrovePos& pos,
                         const Common::String& target,
                         const Common::String& text);
    CommandPtr  changePi(const GrovePos& pos,
                         const Common::String& target,
                         const Common::String& newtext);

    CommandPtr  insertComment(const GrovePos& pos,
                              const Common::String& text);
    CommandPtr  changeComment(const GrovePos& pos,
                              const Common::String& newText);
    
/// Redline commands

    CommandPtr  convertToRedline(const GrovePos& first,
                                 const GrovePos& last,
                                 uint redlineMask,
                                 const Common::String& annotation);
    
    CommandPtr  convertFromRedline(const GrovePos& pos);
    
    CommandPtr  changeRedline(const GrovePos& pos,
                              uint redlineMask,
                              const Common::String& annotation);

    CommandPtr  insertRedline(const GrovePos& pos,
                              uint redlineMask,
                              const Common::String& annotation);

/// XInclude commands

    CommandPtr  insertXinclude(const GrovePos& pos,
                               GroveLib::Grove* targetGrove,
                               bool insertAsText, 
                               const Common::PropertyNode* props);

    CommandPtr  insertXinclude(const GrovePos& pos,
                               const Common::PropertyNode* props,
                               const GroveLib::StripInfo* si = 0, 
                               bool insertAsText = false);
    
    CommandPtr  convertToXinclude(const GrovePos& first,
                                  const GrovePos& last,
                                  const GroveLib::GrovePtr& grove,
                                  const Common::PropertyNode* iprops);
    
    GroveLib::GrovePtr  buildXincludeGrove(const GrovePos& pos, 
                                           const Common::String& url,
                                           const Common::String& encodingStr,
                                           bool isText);

    GroveLib::GrovePtr  buildXincludeGrove(const GrovePos& first,
                                           const GrovePos& last);
    
    GroveLib::Node*     findXincludeNode(const GrovePos& pos, 
                                         GroveLib::Grove* grove,
                                         const Common::String& xpointer, 
                                         bool isText);
    
    ///////////////////////////////////////////////////////////
    
    Common::String  errorMessage() const;
    bool            checkSelection(const GrovePos& from, const GrovePos& to);

protected:
    Common::Command* getDeleteCommand(const GroveLib::NodePtr& node) const;

    GroveEditorMessages::Messages               errorMessage_;
};

} //namespace GroceEditor

#endif // DOC_EDITOR_H
