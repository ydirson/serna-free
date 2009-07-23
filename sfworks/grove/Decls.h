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

#ifndef DECLS_H_
#define DECLS_H_

#include "grove/grove_defs.h"
#include "common/RefCntPtr.h"

GROVE_NAMESPACE_BEGIN

class Grove;
typedef COMMON_NS::RefCntPtr<Grove> GrovePtr;
class Node;
typedef COMMON_NS::RefCntPtr<Node> NodePtr;
class DOMImplementation;
class GroveSectionRoot;
typedef COMMON_NS::CastedRefCntPtr<Node, GroveSectionRoot> GroveSectionRootPtr;
class DocumentFragment;
typedef COMMON_NS::CastedRefCntPtr<Node, DocumentFragment> DocumentFragmentPtr;
class Attr;
typedef COMMON_NS::CastedRefCntPtr<Node, Attr> AttrPtr;
class Text;
typedef COMMON_NS::CastedRefCntPtr<Node, Text> TextPtr;
class Element;
typedef COMMON_NS::CastedRefCntPtr<Node, Element> ElementPtr;
class MarkedSectionStart;
typedef COMMON_NS::CastedRefCntPtr<Node, MarkedSectionStart> MarkedSectionStartPtr;
class MarkedSectionEnd;
typedef COMMON_NS::CastedRefCntPtr<Node, MarkedSectionEnd> MarkedSectionEndPtr;
class EntityReferenceStart;
typedef COMMON_NS::RefCntPtr<EntityReferenceStart> ErsPtr;
typedef COMMON_NS::CastedRefCntPtr<Node, EntityReferenceStart> EntityReferenceStartPtr;
class EntityReferenceEnd;
typedef COMMON_NS::CastedRefCntPtr<Node, EntityReferenceEnd> EntityReferenceEndPtr;
class Comment;
typedef COMMON_NS::CastedRefCntPtr<Node, Comment> CommentPtr;
class ProcessingInstruction;
typedef COMMON_NS::CastedRefCntPtr<Node, ProcessingInstruction> ProcessingInstructionPtr;
class Document;
typedef COMMON_NS::CastedRefCntPtr<Node, Document> DocumentPtr;
class CharacterData;
class DebugGroveVisitor;
class ElementIdTable;
class EntityDecl;
typedef COMMON_NS::RefCntPtr<EntityDecl> EntDeclPtr;
typedef COMMON_NS::CastedRefCntPtr<Node, EntityDecl> EntityDeclPtr;
class EntityDeclExt;
class InternalEntityDecl;
class ExternalEntityDecl;
class EntityDeclSet;
class EntityReferenceTable;
class PublicId;
class ExternalId;
class GroveArgs;
class GroveSaver;
class GroveSectionRoot;
class GroveVisitorBase;
class GroveVisitor;
class GroveVisitorMux;
class Invalidity;
class StringInvalidity;
class MessageInvalidity;
class NodeInvalidity;
class NodeVisitor;
class OutputProperties;
class DocumentProlog;
typedef COMMON_NS::CastedRefCntPtr<Node, DocumentProlog> DocumentPrologPtr;
class EntityDeclNode;
class AttrDeclNode;
class ElementDeclNode;
class SSepNode;
class QualifiedName;
class ExpandedName;
class NodeWithNamespace;
class StripInfo;

GROVE_NAMESPACE_END

#endif // DECLS_H_
