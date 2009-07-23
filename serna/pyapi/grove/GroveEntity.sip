// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//
class GroveEntityDecl : SimpleWrappedObject {
%TypeHeaderCode
#include "sapi/grove/GroveEntity.h"
using namespace SernaApi;
%End

public:
    enum DeclType {
        invalidDeclType,
        internalGeneralEntity,   externalGeneralEntity,
        internalParameterEntity, externalParameterEntity,
        notation, doctype, pi, subdoc, defaulted, document,
        xinclude, undefined
    };
    enum DataType {
        invalidDataType,
        cdata, sdata, ndata, sgml
    };
    enum DeclOrigin {
        invalidDeclOrigin, prolog, dtd, schema, sd, special
    };
    DeclOrigin          declOrigin() const;
    SString             name() const;
    DeclType            declType() const;
    DataType            dataType() const;
    bool                isReadOnly() const;

    GroveInternalEntityDecl asGroveInternalEntityDecl() const;
    GroveExternalEntityDecl asGroveExternalEntityDecl() const;
    GroveXincludeDecl       asGroveXincludeDecl() const;
    GroveNotationDecl       asGroveNotationDecl() const;

private:
    GroveEntityDecl();
};
    
class GroveInternalEntityDecl : GroveEntityDecl {
public:
    SString     content() const;

private:
    GroveInternalEntityDecl();
};

class GroveExternalEntityDecl : GroveEntityDecl {
public:
    SString     sysid() const;
    SString     encoding() const;
    SString     notationName() const;    

private:
    GroveExternalEntityDecl();
};

class GroveXincludeDecl : GroveEntityDecl {
public:
    SString     url() const;
    SString     expr() const;
    bool        isFallback() const;

private:
    GroveXincludeDecl();
};

class GroveNotationDecl : GroveEntityDecl {
public:
    SString     sysid() const;

private:
    GroveNotationDecl();
};

class GroveEntityDeclSet : SimpleWrappedObject {
public:
    GroveEntityDecl     lookupDecl(const SString&) const;
    class Iterator : SimpleWrappedObject {
    public:
        bool            ok() const;
        GroveEntityDecl next();
    private:
        Iterator();
    };
    Iterator            first() const;

private:
    GroveEntityDeclSet();
};

class GroveEntityReferenceTable : SimpleWrappedObject {
public:
    class ErtEntry : SimpleWrappedObject {
    public:
        unsigned int    numOfRefs() const;
        GroveErs        node(unsigned int) const;
    private:
        ErtEntry();
    };
    ErtEntry        lookupErs(const GroveEntityDecl&) const;    
    
private:
    GroveEntityReferenceTable();
};

