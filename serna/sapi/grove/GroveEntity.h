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
#ifndef SAPI_GROVE_ENTITY_H_
#define SAPI_GROVE_ENTITY_H_

#include "sapi/sapi_defs.h"
#include "sapi/common/SString.h"
#include "sapi/common/WrappedObject.h"
#include "sapi/grove/GroveDecls.h"

namespace SernaApi {

/// Base class for all entity declarations (internal, external, xinclude)
class SAPI_EXPIMP GroveEntityDecl : public RefCountedWrappedObject {
public:
    GroveEntityDecl(SernaApiBase*);

    enum DeclType {
        invalidDeclType,
        internalGeneralEntity,   externalGeneralEntity,
        internalParameterEntity, externalParameterEntity,
        notation, doctype, pi, subdoc, defaulted, document,
        xinclude, undefined
    };
    enum DeclOrigin {
        invalidDeclOrigin, prolog, dtd, schema, sd, special
    };
    /// Entity name
    SString name() const;

    /// Entity data type (CDATA, SDATA, NDATA, parsed text (SGML)).
    enum DataType {
        invalidDataType,
        cdata, sdata, ndata, sgml
    };

    /// Declaration type
    DeclType            declType() const;

    /// Entity content data type
    DataType            dataType() const;

    /// Declaration origin flags
    DeclOrigin          declOrigin() const;
    
    /// Returns True if the content of current entity is read-only
    bool                isReadOnly() const;

    GroveInternalEntityDecl asGroveInternalEntityDecl() const;
    GroveExternalEntityDecl asGroveExternalEntityDecl() const;
    GroveXincludeDecl       asGroveXincludeDecl() const;
    GroveNotationDecl       asGroveNotationDecl() const;
};

/// Internal entity declaration
class SAPI_EXPIMP GroveInternalEntityDecl : public GroveEntityDecl {
public:
    GroveInternalEntityDecl(SernaApiBase*);
    
    /// Internal entity content
    SString     content() const;
};

/// External entity declaration
class SAPI_EXPIMP GroveExternalEntityDecl : public GroveEntityDecl {
public:
    GroveExternalEntityDecl(SernaApiBase*);
    
    /// Returns entity SYSID
    SString     sysid() const;

    /// Returns string representation of entity encoding
    SString     encoding() const;

    /// Returns notation (NDATA) name, if any
    SString     notationName() const;    
};

/// XInclude declaration (used by XInclude references only)
class SAPI_EXPIMP GroveXincludeDecl : public GroveEntityDecl {
public:
    GroveXincludeDecl(SernaApiBase*);

    /// Returns XInclude href URL
    SString     url() const;
    
    /// Returns XInclude 'pointer' expression, if any
    SString     expr() const;

    /// Returns True if the current XInclude is in fallback state
    bool        isFallback() const;
};

/// Notation declaration
class SAPI_EXPIMP GroveNotationDecl : public GroveEntityDecl {
public:
    GroveNotationDecl(SernaApiBase*);

    SString     sysid() const;
};

/////////////////////////////////////////////////////////////////

class SAPI_EXPIMP GroveEntityDeclSet : public SimpleWrappedObject {
public:
    GroveEntityDeclSet(SernaApiBase*);
    /// Iterator for traversing entity declarations. 
    /// Use: while (iter.next()) { do_something(iter.current()); }
    class SAPI_EXPIMP Iterator : public RefCountedWrappedObject {
    public:
        Iterator(SernaApiBase* = 0);
        bool            ok() const;
        GroveEntityDecl next();
    };
    /// Returns iterator pointing to the first entry in the entity
    /// declaration table
    Iterator            first() const;

    /// Lookup entity declaration by name
    GroveEntityDecl     lookupDecl(const SString& name) const;
};

class SAPI_EXPIMP GroveEntityReferenceTable : public SimpleWrappedObject {
public:
    class SAPI_EXPIMP ErtEntry : public SimpleWrappedObject {
    public:
        ErtEntry(SernaApiBase*);
        unsigned int    numOfRefs() const;
        GroveErs        node(unsigned int index) const;
    };
    GroveEntityReferenceTable(SernaApiBase*);

    /// Returns first entity reference for given entity declaration
    ErtEntry        lookupErs(const GroveEntityDecl&) const;    
};

} // namespace SernaApi

#endif // SAPI_GROVE_ENTITY_H_
