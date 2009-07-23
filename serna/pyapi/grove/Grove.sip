// (c) 2004 by Syntext, Inc.
//
// This is a copyrighted commercial software.
//

class CatalogManager : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/grove/CatalogManager.h"
using namespace SernaApi;
%End
public:
    CatalogManager(const CatalogManager&);
    SString         getCatalogsList() const; 
};

class Grove : SimpleWrappedObject {

%TypeHeaderCode
#include "sapi/grove/Grove.h"
using namespace SernaApi;
%End

public:

    Grove(const SString&);
    Grove();
    virtual ~Grove();
    GroveDocument       document() const;
    GroveIdManager      idManager();
    SString             doctypeName() const;
    SString             topSysid() const;
    GroveEntityDeclSet  entityDecls() const;
    GroveEntityDeclSet  parameterEntityDecls() const;
    GroveEntityDeclSet  notations() const;

    enum GroveBuilderFlags {
        SGML,
        noIdProcessing,
        noSSep,
        noProlog,
        noMarkedSections,
        noEntitySections,
        noSections,
        noComments,
        pureXmlData,
        noAttrGroups,
        noExtSubsetDecls,
        lineInfo
    };

    static Grove buildGroveFromString(const SString&,
                                      GroveBuilderFlags = pureXmlData,
                                      bool = false,
                                      const CatalogManager& = CatalogManager());
    static Grove buildGroveFromFile(const SString&,
                                    GroveBuilderFlags = pureXmlData,
                                    bool = false,
                                    const CatalogManager& = CatalogManager());
    enum SaveFlags {
        GS_SAVE_PROLOG      = 0001, //! Save prolog
        GS_SAVE_CONTENT     = 0002, //! Save content
        GS_SAVE_ENTITIES    = 0004, //! Save external entities
        GS_EXPAND_ENTITIES  = 0010, //! Expand entities
        GS_SAVE_DEFATTRS    = 0020, //! Save with default attribute values
        GS_FORCE_SAVE       = 0040, //! Save even unmodified entities
        GS_RECURSIVE        = 0100, //! Save all sub-groves, if any
        GS_INDENT           = 0200, //! Indent output
        GS_DONTSAVEDEFENC   = 0400, //! don't always save default encoding
        GS_DEF_FILEFLAGS    = 0107,
            // (GS_SAVE_PROLOG|GS_SAVE_CONTENT|GS_SAVE_ENTITIES|GS_RECURSIVE),
        GS_DEF_STRFLAGS     = 0003
            //(GS_SAVE_PROLOG|GS_SAVE_CONTENT)
    };
    bool    saveAsXmlFile(int = GS_DEF_FILEFLAGS,
                          const GroveStripInfo& = GroveStripInfo(),
                          const SString& = SString());
    bool    saveAsXmlString(SString&,
                            int = GS_DEF_STRFLAGS,
                            const GroveStripInfo& = GroveStripInfo());
};

