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

#ifndef X_NODE_PTR_H_
#define X_NODE_PTR_H_

#include "grove/grove_defs.h"
#include "common/common_defs.h"
#include "grove/Decls.h"

GROVE_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////
// Node type conversions
//
// Note: this part depends on COMMON_NAMESPACE::RefCntPtr<T> implementation!
//
////////////////////////////////////////////////////////////////////////

#ifdef GROVE_NAMESPACE
# define FORCE_GROVE_NAMESPACE_(t)  GROVE_NAMESPACE::t
#else
# define FORCE_GROVE_NAMESPACE_(t)  t
#endif // GROVE_NAMESPACE

#ifdef GROVE_DEBUG_CAST
# define XNODEPTR_NTYPE_CAST_       dynamic_cast
# define XNODEPTR_NODE_CAST_(t,v)   SAFE_CAST(FORCE_GROVE_NAMESPACE_(t),v)
# define C_XNODEPTR_NODE_CAST_(t,v) \
     SAFE_CAST(const FORCE_GROVE_NAMESPACE_(t),v)
#else
# define XNODEPTR_NTYPE_CAST_       static_cast
# define XNODEPTR_NODE_CAST_(t,v) \
     (static_cast<FORCE_GROVE_NAMESPACE_(t)>(v))
# define C_XNODEPTR_NODE_CAST_(t,v) \
     (static_cast<const FORCE_GROVE_NAMESPACE_(t)>(v))
#endif

#define DOCUMENT_FRAGMENT_CAST(n)   XNODEPTR_NODE_CAST_(DocumentFragment*, n)
#define ATTR_CAST(n)                XNODEPTR_NODE_CAST_(Attr*, n)
#define TEXT_CAST(n)                XNODEPTR_NODE_CAST_(Text*, n)
#define ELEMENT_CAST(n)             XNODEPTR_NODE_CAST_(Element*, n)
#define MSS_CAST(n)                 XNODEPTR_NODE_CAST_(MarkedSectionStart*, n)
#define MSE_CAST(n)                 XNODEPTR_NODE_CAST_(MarkedSectionEnd*, n)
#define ERS_CAST(n)                 XNODEPTR_NODE_CAST_(EntityReferenceStart*, n)
#define ERE_CAST(n)                 XNODEPTR_NODE_CAST_(EntityReferenceEnd*, n)
#define PI_CAST(n)                  XNODEPTR_NODE_CAST_(ProcessingInstruction*, n)
#define ENTITY_DECL_NODE_CAST(n)    XNODEPTR_NODE_CAST_(EntityDeclNode*, n)
#define ELEMENT_DECL_NODE_CAST(n)   XNODEPTR_NODE_CAST_(ElementDeclNode*, n)
#define ATTR_DECL_NODE_CAST(n)      XNODEPTR_NODE_CAST_(AttrDeclNode*, n)
#define SSEP_NODE_CAST(n)           XNODEPTR_NODE_CAST_(SSepNode*, n)
#define COMMENT_CAST(n)             XNODEPTR_NODE_CAST_(Comment*, n)
#define PROMISE_CAST(n)             XNODEPTR_NODE_CAST_(ChoicePromise*, n)

#define CONST_DOCUMENT_FRAGMENT_CAST(n) C_XNODEPTR_NODE_CAST_(DocumentFragment*, n)
#define CONST_ATTR_CAST(n)          C_XNODEPTR_NODE_CAST_(Attr*, n)
#define CONST_TEXT_CAST(n)          C_XNODEPTR_NODE_CAST_(Text*, n)
#define CONST_ELEMENT_CAST(n)       C_XNODEPTR_NODE_CAST_(Element*, n)
#define CONST_MSS_CAST(n)           C_XNODEPTR_NODE_CAST_(MarkedSectionStart*, n)
#define CONST_MSE_CAST(n)           C_XNODEPTR_NODE_CAST_(MarkedSectionEnd*, n)
#define CONST_ERS_CAST(n)           C_XNODEPTR_NODE_CAST_(EntityReferenceStart*, n)
#define CONST_ERE_CAST(n)           C_XNODEPTR_NODE_CAST_(EntityReferenceEnd*, n)
#define CONST_PI_CAST(n)            C_XNODEPTR_NODE_CAST_(ProcessingInstruction*, n)
#define CONST_ENTITY_DECL_NODE_CAST(n) C_XNODEPTR_NODE_CAST_(EntityDeclNode*, n)
#define CONST_ELEMENT_DECL_NODE_CAST(n) C_XNODEPTR_NODE_CAST_(ElementDeclNode*, n)
#define CONST_ATTR_DECL_NODE_CAST(n) C_XNODEPTR_NODE_CAST_(AttrDeclNode*, n)
#define CONST_SSEP_NODE_CAST(n)     C_XNODEPTR_NODE_CAST_(SSepNode*, n)
#define CONST_COMMENT_CAST(n)       C_XNODEPTR_NODE_CAST_(Comment*, n)
#define CONST_PROMISE_CAST(n)       C_XNODEPTR_NODE_CAST_(ChoicePromise*, n)

GROVE_NAMESPACE_END

#endif // X_NODE_PTR_H_
